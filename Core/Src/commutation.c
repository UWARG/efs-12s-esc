#include "commutation.h"
#include "pwm_driver.h"
#include "bemf.h"

/* ========= Configuration ========= */

/* Open-loop startup timing (ticks) */
#define STARTUP_INITIAL_PERIOD   4000
#define STARTUP_FINAL_PERIOD     800
#define STARTUP_RAMP_STEPS       200

/* Blanking time after commutation before BEMF valid */
#define COMMUTATION_BLANKING_TICKS  200

#define THROTTLE_MINIMUM_START   48  /* Minimum throttle to start motor */
#define THROTTLE_MAXIMUM        2047

/* ========= Internal State ========= */

static comm_step_t step;
static motor_state_t state;

static uint32_t next_comm_ticks;
static uint32_t last_comm_ticks;
static uint32_t last_zc_ticks;

static uint32_t startup_period;
static uint16_t startup_step;

/* ========= Commutation Table ========= */
/*
 * Each step defines:
 *  - High-side ON
 *  - Low-side ON
 *  - Floating phase
 *
 * Convention:
 *  Phase A = CH1
 *  Phase B = CH2
 *  Phase C = CH3
 */

typedef struct {
    phase_t high;
    phase_t low;
    phase_t floating;
} comm_entry_t;

static const comm_entry_t comm_table[6] = {
    /* HS     LS     FLOAT */
    { PHASE_A, PHASE_B, PHASE_C }, /* Step 0 */
    { PHASE_A, PHASE_C, PHASE_B }, /* Step 1 */
    { PHASE_B, PHASE_C, PHASE_A }, /* Step 2 */
    { PHASE_B, PHASE_A, PHASE_C }, /* Step 3 */
    { PHASE_C, PHASE_A, PHASE_B }, /* Step 4 */
    { PHASE_C, PHASE_B, PHASE_A }  /* Step 5 */
};

/* ========= Internal Helpers ========= */

static void apply_step(comm_step_t s)
{
    const comm_entry_t *e = &comm_table[s];

    pwm_set_phase_state(e->high, true);
    pwm_set_phase_state(e->low,  false);
    pwm_set_phase_floating(e->floating);

    last_comm_ticks = next_comm_ticks;
}

static void advance_step(void)
{
    step = (comm_step_t)((step + 1) % 6);
    apply_step(step);
}

/* ========= Public API ========= */

void commutation_init(void)
{
    step = COMM_STEP_0;
    state = MOTOR_STOPPED;

    startup_period = STARTUP_INITIAL_PERIOD;
    startup_step = 0;

    pwm_all_off();
    bemf_reset();
}

void commutation_start(void)
{
    state = MOTOR_OPEN_LOOP;

    step = COMM_STEP_0;
    startup_period = STARTUP_INITIAL_PERIOD;
    startup_step = 0;

    next_comm_ticks = 0;
    last_comm_ticks = 0;

    apply_step(step);
}

void commutation_stop(void)
{
    state = MOTOR_STOPPED;
    pwm_all_off();
}

motor_state_t commutation_state(void)
{
    return state;
}

phase_t commutation_floating_phase(void)
{
    return comm_table[step].floating;
}

void commutation_update(uint32_t now_ticks)
{
    if (state == MOTOR_STOPPED || state == MOTOR_FAULT)
        return;

    /* -------- Open-loop startup -------- */
    if (state == MOTOR_OPEN_LOOP) {
        if ((int32_t)(now_ticks - next_comm_ticks) >= 0) {

            advance_step();

            /* Linear ramp */
            startup_period -=
                (STARTUP_INITIAL_PERIOD - STARTUP_FINAL_PERIOD)
                / STARTUP_RAMP_STEPS;

            if (startup_period < STARTUP_FINAL_PERIOD)
                startup_period = STARTUP_FINAL_PERIOD;

            next_comm_ticks = now_ticks + startup_period;
            startup_step++;

            /* Transition to closed-loop */
            if (startup_step >= STARTUP_RAMP_STEPS) {
                state = MOTOR_CLOSED_LOOP;
                bemf_reset();
            }
        }
    }

    /* -------- Closed-loop -------- */
    else if (state == MOTOR_CLOSED_LOOP) {
        if ((int32_t)(now_ticks - next_comm_ticks) >= 0) {
            advance_step();
        }
    }
}

void commutation_on_zc(uint32_t zc_ticks)
{
    if (state != MOTOR_CLOSED_LOOP)
        return;

    if ((zc_ticks - last_comm_ticks) < COMMUTATION_BLANKING_TICKS)
        return;

    if (last_zc_ticks == 0) {
        last_zc_ticks = zc_ticks;
        return; // need two ZCs to estimate speed
    }

    uint32_t T60 = zc_ticks - last_zc_ticks;
    uint32_t T30 = T60 / 2;

    last_zc_ticks = zc_ticks;
    next_comm_ticks = zc_ticks + T30;
}

void commutation_reset(void)
{
    commutation_stop();
    commutation_init();
}

static uint16_t throttle_to_duty(uint16_t t)
{
    if (t < THROTTLE_MINIMUM_START)
        return 0;

    t -= THROTTLE_MINIMUM_START; // normalize
    return (t * PWM_MAX_DUTY) / (THROTTLE_MAXIMUM - THROTTLE_MINIMUM_START);
}

void commutation_set_throttle(uint16_t throttle)
{
    /* For now: map throttle to startup */
    if (throttle == 0) {
        commutation_stop();
        return;
    }

    if (state == MOTOR_STOPPED) {
        commutation_start();
    }

    uint16_t duty = throttle_to_duty(throttle);
    pwm_set_duty(duty);
}
