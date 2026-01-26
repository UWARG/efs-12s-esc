#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t throttle;
    uint8_t  telemetry;
    bool     valid;
} dshot_frame_t;

void dshot_init(void);
bool dshot_frame_available(void);
bool dshot_get_frame(dshot_frame_t *out);
void dshot_ic_edge(uint32_t capture);

