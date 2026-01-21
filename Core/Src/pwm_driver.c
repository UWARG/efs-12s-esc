#include "pwm_driver.h"
#include "stm32f0xx_hal.h"

/* ========= Configuration ========= */

#define PWM_MAX_DUTY     1000
#define PWM_TIMER       TIM1

/* ========= External Handles ========= */
extern TIM_HandleTypeDef htim1;

/* ========= Internal State ========= */
static uint16_t pwm_duty = 0;

/* ========= Helpers ========= */

static inline void set_compare(TIM_TypeDef *tim,
                               uint32_t channel,
                               uint16_t duty)
{
    switch (channel) {
    case TIM_CHANNEL_1: tim->CCR1 = duty; break;
    case TIM_CHANNEL_2: tim->CCR2 = duty; break;
    case TIM_CHANNEL_3: tim->CCR3 = duty; break;
    default: break;
    }
}

static inline void enable_channel(uint32_t ch, bool enable_main, bool enable_n)
{
    if (enable_main)
        PWM_TIMER->CCER |=  (1 << (ch * 4));
    else
        PWM_TIMER->CCER &= ~(1 << (ch * 4));

    if (enable_n)
        PWM_TIMER->CCER |=  (1 << (ch * 4 + 2));
    else
        PWM_TIMER->CCER &= ~(1 << (ch * 4 + 2));
}

static uint32_t phase_to_channel(phase_t p)
{
    switch (p) {
    case PHASE_A: return TIM_CHANNEL_1;
    case PHASE_B: return TIM_CHANNEL_2;
    case PHASE_C: return TIM_CHANNEL_3;
    default:      return TIM_CHANNEL_1;
    }
}

/* ========= Public API ========= */

void pwm_init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    pwm_all_off();
}

void pwm_all_off(void)
{
    __disable_irq();

    PWM_TIMER->CCER = 0;
    PWM_TIMER->CCR1 = 0;
    PWM_TIMER->CCR2 = 0;
    PWM_TIMER->CCR3 = 0;

    __enable_irq();
}

void pwm_set_duty(uint16_t duty)
{
    if (duty > PWM_MAX_DUTY)
        duty = PWM_MAX_DUTY;

    pwm_duty = duty;
}

void pwm_set_phase_state(phase_t phase, bool high_side)
{
    uint32_t ch = phase_to_channel(phase);

    __disable_irq();

    if (high_side) {
        /* HS ON, LS OFF */
        set_compare(PWM_TIMER, ch, pwm_duty);
        enable_channel(ch, true, false);
    } else {
        /* LS ON, HS OFF */
        set_compare(PWM_TIMER, ch, pwm_duty);
        enable_channel(ch, false, true);
    }

    __enable_irq();
}

void pwm_set_phase_floating(phase_t phase)
{
    uint32_t ch = phase_to_channel(phase);

    __disable_irq();

    set_compare(PWM_TIMER, ch, 0);
    enable_channel(ch, false, false);

    __enable_irq();
}
