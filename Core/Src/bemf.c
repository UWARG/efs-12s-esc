#include "bemf.h"

/* ========= Configuration ========= */

/* Simple IIR low-pass filter strength
 * y[n] = y[n-1] + (x − y[n-1]) >> BEMF_FILTER_SHIFT
 */
#define BEMF_FILTER_SHIFT   3   /* 1/8 smoothing */

/* Minimum BEMF magnitude to accept a zero-cross (ADC counts) */
#define BEMF_ZC_THRESHOLD   10

/* ========= Internal State ========= */

typedef struct {
    int16_t filtered;
    int16_t prev;
} bemf_filter_t;

static bemf_filter_t bemf_filt[3];
static bool zc_pending;
static bemf_event_t zc_event;

static inline int16_t iir_filter(int16_t prev, int16_t input)
{
    return prev + ((input - prev) >> BEMF_FILTER_SHIFT);
}

static inline int16_t phase_voltage(phase_t p, const adc_sample_t *s)
{
    switch (p) {
    case PHASE_A: return (int16_t)s->a_vsense;
    case PHASE_B: return (int16_t)s->b_vsense;
    case PHASE_C: return (int16_t)s->c_vsense;
    default:      return 0;
    }
}

/* ========= Public API ========= */

void bemf_init(void)
{
    bemf_reset();
}

void bemf_reset(void)
{
    for (int i = 0; i < 3; i++) {
        bemf_filt[i].filtered = 0;
        bemf_filt[i].prev     = 0;
    }
    zc_pending = false;
}

void bemf_update(phase_t floating_phase,
                 const adc_sample_t *adc,
                 uint32_t now_ticks)
{
    /* Compute virtual neutral = (A + B + C) / 3 */
    int16_t neutral =
        ((int16_t)adc->a_vsense +
         (int16_t)adc->b_vsense +
         (int16_t)adc->c_vsense) / 3;

    /* Raw BEMF of floating phase */
    int16_t raw_bemf = phase_voltage(floating_phase, adc) - neutral;

    /* Filter */
    bemf_filter_t *f = &bemf_filt[floating_phase];
    f->filtered = iir_filter(f->filtered, raw_bemf);

    /* Zero-cross detection:
     * sign change across zero with minimum amplitude
     */
    int16_t prev = f->prev;
    int16_t curr = f->filtered;
    f->prev = curr;

    if (zc_pending)
        return;

    if ((prev < 0 && curr >= 0) ||
        (prev > 0 && curr <= 0)) {

        if ((prev >  BEMF_ZC_THRESHOLD) ||
            (prev < -BEMF_ZC_THRESHOLD)) {

            zc_event.phase     = floating_phase;
            zc_event.timestamp = now_ticks;
            zc_event.bemf_value = curr;
            zc_pending = true;
        }
    }
}

bool bemf_zc_available(void)
{
    return zc_pending;
}

bool bemf_get_zc(bemf_event_t *event)
{
    if (!zc_pending)
        return false;

    *event = zc_event;
    zc_pending = false;
    return true;
}
