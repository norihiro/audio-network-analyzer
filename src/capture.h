#pragma once

#include <stdint.h>
#include "context.h"

typedef struct capture_s capture_t;

capture_t *capture_create(audio_context_t *);
void capture_release(capture_t *);

int capture_measure(capture_t *c, double *real, double *imag);
uint64_t capture_ts(capture_t *);
