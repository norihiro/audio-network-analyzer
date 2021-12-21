#pragma once

#include <stdint.h>

typedef struct audio_context_s audio_context_t;

audio_context_t *audio_context_create();
void audio_context_addref(audio_context_t *);
void audio_context_release(audio_context_t *);

struct data_entry_t
{
	double freq;
	double real, imag;
	uint64_t ts_begin_src, ts_begin_cap, ts_end;
};
