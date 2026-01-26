#ifndef COMMUTATION_H
#define COMMUTATION_H

#include <stdint.h>
#include <stdbool.h>
#include "bemf.h"

/* ========= Types ========= */

/* 6-step commutation states */
typedef enum {
    COMM_STEP_0 = 0,
    COMM_STEP_1,
    COMM_STEP_2,
    COMM_STEP_3,
    COMM_STEP_4,
    COMM_STEP_5
} comm_step_t;

/* High-level motor state */
typedef enum {
    MOTOR_STOPPED = 0,
    MOTOR_OPEN_LOOP,
    MOTOR_CLOSED_LOOP,
    MOTOR_FAULT
} motor_state_t;

typedef enum {
    ESC_DISARMED,
    ESC_ARMING,
    ESC_RUNNING,
    ESC_FAULT
} esc_state_t;


/* ========= API ========= */

/* Initialize commutation module */
void commutation_init(void);

/* Start motor in open-loop */
void commutation_start(void);

/* Stop motor immediately */
void commutation_stop(void);

/* Called periodically from main loop */
void commutation_update(uint32_t now_ticks);

/* Called when BEMF zero-cross occurs */
void commutation_on_zc(uint32_t zc_ticks);

/* Get current floating phase */
phase_t commutation_floating_phase(void);

/* Get motor state */
motor_state_t commutation_state(void);

#endif /* COMMUTATION_H */
