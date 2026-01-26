#include "dshot.h"
#include "stm32f051x8.h"
#include <stdbool.h>

/* ================= Configuration ================= */

#define DSHOT_BITS                 16
#define DSHOT_MAX_EDGES            (DSHOT_BITS * 3)

/* Timer clock = 8 MHz → 125 ns / tick */
#define DSHOT_FRAME_GAP_TICKS      300   /* ~37 µs gap resets frame */
#define DSHOT_ONE_THRESHOLD_PCT    60    /* >60% duty = '1' */

/* ================= Internal State ================= */

static volatile uint32_t rise_times[DSHOT_BITS];
static volatile uint32_t fall_times[DSHOT_BITS];
static volatile uint8_t  bit_index;

static volatile bool frame_ready;
static volatile dshot_frame_t last_frame;

/* Edge tracking */
static uint32_t last_capture;
static uint32_t last_rise;
static bool waiting_for_fall;

/* ================= Utilities ================= */

static inline uint32_t ticks_delta(uint32_t now, uint32_t prev)
{
    return (now >= prev) ? (now - prev) : (0x10000u + now - prev);
}

static uint8_t dshot_checksum(uint16_t payload)
{
    uint8_t csum = 0;
    for (int i = 0; i < 3; i++) {
        csum ^= payload & 0xF;
        payload >>= 4;
    }
    return csum & 0xF;
}

/* ================= Decoder ================= */

static void dshot_decode_frame(void)
{
    uint16_t raw = 0;

    for (int i = 0; i < DSHOT_BITS; i++) {
        uint32_t high = ticks_delta(fall_times[i], rise_times[i]);

        uint32_t bit_period;
        if (i < DSHOT_BITS - 1)
            bit_period = ticks_delta(rise_times[i + 1], rise_times[i]);
        else
            bit_period = high * 2; /* fallback for last bit */

        raw <<= 1;
        if ((high * 100) > (bit_period * DSHOT_ONE_THRESHOLD_PCT))
            raw |= 1;
    }

    uint16_t payload = raw >> 4;
    uint8_t recv_crc = raw & 0xF;

    if (dshot_checksum(payload) != recv_crc)
        return;

    last_frame.throttle  = payload >> 1;
    last_frame.telemetry = payload & 0x1;
    last_frame.valid     = true;
    frame_ready          = true;
}

/* ================= Public API ================= */

void dshot_init(void)
{
    bit_index        = 0;
    frame_ready      = false;
    last_frame.valid = false;
    waiting_for_fall = false;
    last_capture     = 0;
}

bool dshot_frame_available(void)
{
    return frame_ready;
}

bool dshot_get_frame(dshot_frame_t *out)
{
    if (!frame_ready)
        return false;

    *out = last_frame;
    frame_ready = false;
    return true;
}

void dshot_ic_edge(uint32_t cap)
{
    uint32_t delta = ticks_delta(cap, last_capture);
    last_capture = cap;

    /* Frame gap detection */
    if (delta > DSHOT_FRAME_GAP_TICKS) {
        bit_index = 0;
        waiting_for_fall = false;
        return;
    }

    if (!waiting_for_fall) {
        if (bit_index >= DSHOT_BITS)
            return;

        rise_times[bit_index] = cap;
        last_rise = cap;
        waiting_for_fall = true;
    } else {
        fall_times[bit_index] = cap;
        bit_index++;
        waiting_for_fall = false;

        if (bit_index >= DSHOT_BITS) {
            dshot_decode_frame();
            bit_index = 0;
        }
    }
}
