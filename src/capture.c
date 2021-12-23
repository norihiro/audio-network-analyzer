#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <pulse/error.h>

#include "capture.h"
#include "context.h"
#include "context-internal.h"

struct capture_s
{
	volatile int refcnt;
	audio_context_t *ctx;
	pa_stream *stream;

	int sum_n;
	double sum_real, sum_imag;
	int i_sample;
	bool started;

	uint64_t n_read;
	int i_data;
};

static void capture_callback(pa_stream *p, size_t nbytes, void *userdata);

capture_t *capture_create(audio_context_t *ctx)
{
	capture_t *c = calloc(1, sizeof(capture_t));
	int ret;

	audio_context_addref(ctx);
	c->ctx = ctx;
	c->stream = pa_stream_new(ctx->pa, "audio-network-analyzer-capture", &ctx->spec, NULL);
	if (!c->stream) {
		fprintf(stderr, "Error: pa_stream_new failed\n");
		goto err;
	}

	ret = pa_stream_connect_record(
			c->stream,
			NULL,
			NULL,
			0 );
	if (ret) {
		fprintf(stderr, "Error: pa_stream_connect_record failed %d %s\n", ret, pa_strerror(ret));
		goto err;
	}

	pa_stream_set_read_callback(c->stream, capture_callback, c);

	return c;

err:
	if (c->stream)
		pa_stream_unref(c->stream);
	free(c);
	return NULL;
}

static void capture_destroy(capture_t *c)
{
	pa_stream_disconnect(c->stream);
	audio_context_release(c->ctx);
	free(c);
}

void capture_release(capture_t *c)
{
	if (c->refcnt)
		c->refcnt--;
	else
		capture_destroy(c);
}

static void capture_callback(pa_stream *p, size_t nbytes_, void *userdata)
{
	(void)nbytes_; // unused parameter
	capture_t *c = userdata;

	const void *data;
	size_t nbytes;
	pa_stream_peek(p, &data, &nbytes);

	int n_sample = nbytes / 4;
	const int16_t *buf = data;
	for (int i=0; i<n_sample; i++) {
		if (c->ctx->data[c->i_data].freq <= 0)
			break;
		if (c->n_read == c->ctx->data[c->i_data].ts_begin_src) {
			c->i_sample = 0;
			c->sum_n = 0;
			c->sum_real = 0.0;
			c->sum_imag = 0.0;
		}

		if (c->ctx->data[c->i_data].ts_begin_cap <= c->n_read) {
			double freq = c->ctx->data[c->i_data].freq;
			const int rate = c->ctx->spec.rate;
			double v_real = sin(c->i_sample * freq * 2.0 * M_PI / rate);
			double v_imag = cos(c->i_sample * freq * 2.0 * M_PI / rate);
			double v = (double)buf[i*2+0] + (double)buf[i*2+1];
			c->sum_n += 1;
			c->sum_real += v * v_real / 32767.;
			c->sum_imag += v * v_imag / 32767.;
		}

		++ c->i_sample;

		c->n_read += 1;
		if (c->n_read == c->ctx->data[c->i_data].ts_end) {
			struct data_entry_t *data = c->ctx->data + c->i_data;
			capture_measure(c, &data->real, &data->imag);
			if (c->ctx->print_entry)
				c->ctx->print_entry(data, c->i_data==0);
			c->i_data++;
		}
	}

	pa_stream_drop(p);
}

int capture_measure(capture_t *c, double *real, double *imag)
{
	if (c->sum_n <= 0)
		return 1;

	*real = c->sum_real / c->sum_n;
	*imag = c->sum_imag / c->sum_n;
	return 0;
}

uint64_t capture_ts(capture_t *c)
{
	return c->n_read;
}
