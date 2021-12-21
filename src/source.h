#pragma once

#include <stdint.h>
#include "context.h"

typedef struct source_s source_t;

source_t *source_create(audio_context_t *);
void source_release(source_t *);

int source_start(source_t *s, double frequency);
