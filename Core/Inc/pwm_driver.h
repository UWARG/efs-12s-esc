#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "bemf.h"


#define PWM_MAX_DUTY     1000
#define PWM_TIMER       TIM1
/* ========= API ========= */

/* Initialize TIM1 PWM + GPIOs */
void pwm_init(void);

/* Disable all phases immediately */
void pwm_all_off(void);

/* Set duty cycle (0–1000) */
void pwm_set_duty(uint16_t duty);

/* Enable phase as HIGH-side driven */
void pwm_set_phase_state(phase_t phase, bool high_side);

/* Float a phase (both HS and LS off) */
void pwm_set_phase_floating(phase_t phase);

#endif /* PWM_DRIVER_H */
