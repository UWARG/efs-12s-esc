#ifndef BEMF_H
#define BEMF_H

#include <stdint.h>
#include <stdbool.h>

/* Motor phases */
typedef enum {
    PHASE_A = 0,
    PHASE_B,
    PHASE_C
} phase_t;

/* Zero-cross event */
typedef struct {
    phase_t phase;          /* Floating phase */
    uint32_t timestamp;     /* Timer ticks at zero-cross */
    int16_t bemf_value;     /* Signed BEMF at crossing */
} bemf_event_t;

/* ADC sample layout (MUST match ADC DMA layout) */
typedef struct {
    uint16_t a_vsense;
    uint16_t b_vsense;
    uint16_t c_vsense;

    uint16_t a_isense;
    uint16_t b_isense;
    uint16_t c_isense;

    uint16_t bus_vsense;
} adc_sample_t;

/* Initialize BEMF module */
void bemf_init(void);

/* Called from main loop or fast control loop
 * floating_phase = phase not driven in current commutation step
 * adc            = pointer to stable ADC DMA snapshot
 * now_ticks      = free-running timer (e.g., TIMx->CNT)
 */
void bemf_update(phase_t floating_phase,
                 const adc_sample_t *adc,
                 uint32_t now_ticks);

/* Check if a zero-cross was detected */
bool bemf_zc_available(void);

/* Retrieve latest zero-cross event (clears flag) */
bool bemf_get_zc(bemf_event_t *event);

/* Reset internal filters/state (call on startup or fault) */
void bemf_reset(void);

#endif /* BEMF_H */
