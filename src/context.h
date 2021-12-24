#pragma once

#include <stdint.h>

typedef struct audio_context_s audio_context_t;

audio_context_t *audio_context_create();
void audio_context_addref(audio_context_t *);
void audio_context_release(audio_context_t *);

#define SRC_MUTE_LEFT 1
#define SRC_MUTE_RIGHT 2

struct data_entry_t
{
	double freq;
	double real, imag;
	uint64_t ts_begin_src, ts_begin_cap, ts_end;
	uint32_t flags;
};
