#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <pulse/error.h>

#include "source.h"
#include "context.h"
#include "context-internal.h"

struct source_s
{
	volatile int refcnt;
	audio_context_t *ctx;
	pa_stream *stream;

	int i_sample;

	uint64_t n_written;
	int i_data;
};

static void source_callback(pa_stream *p, size_t nbytes, void *userdata);

source_t *source_create(audio_context_t *ctx)
{
	source_t *s = calloc(1, sizeof(source_t));
	int ret;

	audio_context_addref(ctx);
	s->ctx = ctx;
	s->stream = pa_stream_new(ctx->pa, "audio-network-analyzer-source", &ctx->spec, NULL);
	if (!s->stream) {
		fprintf(stderr, "Error: pa_stream_new failed\n");
		goto err;
	}

	ret = pa_stream_connect_playback(
			s->stream,
			NULL,
			NULL,
			0,
			NULL,
			NULL );
	if (ret) {
		fprintf(stderr, "Error: pa_stream_connect_playback failed %d %s\n", ret, pa_strerror(ret));
		goto err;
	}

	// TODO: write a function
	ctx->sync_stream = s->stream;
	pa_stream_ref(ctx->sync_stream);

	pa_stream_set_write_callback(s->stream, source_callback, s);

	return s;

err:
	if (s->stream)
		pa_stream_unref(s->stream);
	free(s);
	return NULL;
}

static inline void source_destroy(source_t *s)
{
	pa_stream_disconnect(s->stream);
	if (s->ctx->sync_stream == s->stream)
		pa_stream_unref(s->ctx->sync_stream);
	audio_context_release(s->ctx);
	free(s);
}

void source_release(source_t *s)
{
	if (s->refcnt)
		s->refcnt--;
	else
		source_destroy(s);
}

static void source_callback(pa_stream *p, size_t nbytes, void *userdata)
{
	source_t *s = userdata;

	while (nbytes > 0) {
#define N_SAMPLES 1024
		int16_t buf[N_SAMPLES*2];
		int n_sample = nbytes / 4;
		if (n_sample > N_SAMPLES) n_sample = N_SAMPLES;
		for (int i=0; i<n_sample; i++) {
			if (s->n_written == s->ctx->data[s->i_data].ts_begin_src) {
				s->i_sample = 0;
			}

			double freq = s->ctx->data[s->i_data].freq;
			if (freq < 0.0)
				freq = 0.0;
			const int rate = s->ctx->spec.rate;
			double v = sin(s->i_sample * freq * 2.0 * M_PI / rate);

			uint32_t flags = s->ctx->data[s->i_data].flags;

			buf[i*2+0] = flags & SRC_MUTE_LEFT  ? 0 : (int16_t)(32767*v);
			buf[i*2+1] = flags & SRC_MUTE_RIGHT ? 0 : (int16_t)(32767*v);
			++ s->i_sample;

			s->n_written += 1;
			if (s->n_written >= s->ctx->data[s->i_data].ts_end) {
				if (s->ctx->data[s->i_data].freq > 0.0) {
					++ s->i_data;
				}
			}
		}
		pa_stream_write(p, buf, n_sample*4, NULL, 0, PA_SEEK_RELATIVE);
		nbytes -= n_sample*4;
	}
}
