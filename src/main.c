#include <stdio.h>
#include <unistd.h> // sleep
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "source.h"
#include "capture.h"
#include "context.h"
#include "context-internal.h"

struct main_context
{
	audio_context_t *ctx;

	source_t *src;
	bool src_initialized;

	capture_t *cap;
	bool cap_initialized;
};

static void state_callback_ready(struct main_context *mc)
{
	if (!mc->src_initialized) {
		mc->src = source_create(mc->ctx);
		if (!mc->src)
			pa_mainloop_quit(mc->ctx->mainloop, 1);
		mc->src_initialized = true;
	}

	if (!mc->cap_initialized) {
		mc->cap = capture_create(mc->ctx);
		if (!mc->cap)
			pa_mainloop_quit(mc->ctx->mainloop, 1);
	}
}

static void state_callback(pa_context *c, void *userdata)
{
	pa_context_state_t state = pa_context_get_state(c);

	if (state==PA_CONTEXT_READY)
		state_callback_ready(userdata);
}

static void print_entry(const struct data_entry_t *entry, bool first)
{
	if (first)
		printf("# f [Hz]\tgain [dB]\tphase [deg]\n");
	double v = hypot(entry->real, entry->imag);
	double a = atan2(entry->imag, entry->real);
	printf("%f\t%f\t%f\n", entry->freq, log(v) * 20 / log(10), a * 180.0 / M_PI);
}

static struct data_entry_t data_decade[] = {
	{ .freq = 10.0 },
	{ .freq = 20.0 },
	{ .freq = 50.0 },
	{ .freq = 100.0 },
	{ .freq = 200.0 },
	{ .freq = 500.0 },
	{ .freq = 1000.0 },
	{ .freq = 2000.0 },
	{ .freq = 5000.0 },
	{ .freq = 10000.0 },
	{ .freq = 20000.0 },
	{ .freq = -1.0 }
};

static struct data_entry_t data_roland_vmixer[] = {
	{ .freq = 20 },
	{ .freq = 21 },
	{ .freq = 22 },
	{ .freq = 24 },
	{ .freq = 25 },
	{ .freq = 27 },
	{ .freq = 28 },
	{ .freq = 30 },
	{ .freq = 32 },
	{ .freq = 33 },
	{ .freq = 36 },
	{ .freq = 38 },
	{ .freq = 40 },
	{ .freq = 42 },
	{ .freq = 45 },
	{ .freq = 47 },
	{ .freq = 50 },
	{ .freq = 53 },
	{ .freq = 56 },
	{ .freq = 60 },
	{ .freq = 63 },
	{ .freq = 67 },
	{ .freq = 71 },
	{ .freq = 75 },
	{ .freq = 80 },
	{ .freq = 84 },
	{ .freq = 90 },
	{ .freq = 94 },
	{ .freq = 100 },
	{ .freq = 106 },
	{ .freq = 112 },
	{ .freq = 120 },
	{ .freq = 125 },
	{ .freq = 133 },
	{ .freq = 140 },
	{ .freq = 150 },
	{ .freq = 160 },
	{ .freq = 170 },
	{ .freq = 180 },
	{ .freq = 190 },
	{ .freq = 200 },
	{ .freq = 210 },
	{ .freq = 224 },
	{ .freq = 237 },
	{ .freq = 250 },
	{ .freq = 266 },
	{ .freq = 280 },
	{ .freq = 300 },
	{ .freq = 315 },
	{ .freq = 335 },
	{ .freq = 355 },
	{ .freq = 376 },
	{ .freq = 400 },
	{ .freq = 422 },
	{ .freq = 450 },
	{ .freq = 473 },
	{ .freq = 500 },
	{ .freq = 530 },
	{ .freq = 560 },
	{ .freq = 600 },
	{ .freq = 630 },
	{ .freq = 670 },
	{ .freq = 710 },
	{ .freq = 750 },
	{ .freq = 800 },
	{ .freq = 840 },
	{ .freq = 900 },
	{ .freq = 944 },
	{ .freq = 1.00e3 },
	{ .freq = 1.06e3 },
	{ .freq = 1.12e3 },
	{ .freq = 1.20e3 },
	{ .freq = 1.25e3 },
	{ .freq = 1.33e3 },
	{ .freq = 1.40e3 },
	{ .freq = 1.50e3 },
	{ .freq = 1.60e3 },
	{ .freq = 1.70e3 },
	{ .freq = 1.80e3 },
	{ .freq = 1.90e3 },
	{ .freq = 2.00e3 },
	{ .freq = 2.10e3 },
	{ .freq = 2.24e3 },
	{ .freq = 2.37e3 },
	{ .freq = 2.50e3 },
	{ .freq = 2.66e3 },
	{ .freq = 2.80e3 },
	{ .freq = 3.00e3 },
	{ .freq = 3.15e3 },
	{ .freq = 3.35e3 },
	{ .freq = 3.55e3 },
	{ .freq = 3.76e3 },
	{ .freq = 4.00e3 },
	{ .freq = 4.22e3 },
	{ .freq = 4.50e3 },
	{ .freq = 4.73e3 },
	{ .freq = 5.00e3 },
	{ .freq = 5.30e3 },
	{ .freq = 5.60e3 },
	{ .freq = 6.00e3 },
	{ .freq = 6.30e3 },
	{ .freq = 6.70e3 },
	{ .freq = 7.10e3 },
	{ .freq = 7.50e3 },
	{ .freq = 8.00e3 },
	{ .freq = 8.40e3 },
	{ .freq = 9.00e3 },
	{ .freq = 9.44e3 },
	{ .freq = 10.0e3 },
	{ .freq = 10.6e3 },
	{ .freq = 11.2e3 },
	{ .freq = 12.0e3 },
	{ .freq = 12.5e3 },
	{ .freq = 13.3e3 },
	{ .freq = 14.0e3 },
	{ .freq = 15.0e3 },
	{ .freq = 16.0e3 },
	{ .freq = 17.0e3 },
	{ .freq = 18.0e3 },
	{ .freq = 19.0e3 },
	{ .freq = 20.0e3 },
	{ .freq = -1.0 }
};

#define DEFAULT_DATA data_decade

int main(int argc, char **argv)
{
	struct main_context mc = {0};
	struct data_entry_t *data = NULL;
	double period_sec = 1.25;
	double guardband_sec = 0.25;
	int min_cycle = 8;
	uint64_t ts_end = 0;

	for (int i=1; i<argc; i++) {
		char *ai = argv[i];
		if (!strcmp(ai, "--freq-decade")) {
			data = data_decade;
		}
		else if (!strcmp(ai, "--freq-vmixer")) {
			data = data_roland_vmixer;
		}
		else if (!strcmp(ai, "--period")) {
			if (i+1 >= argc) {
				fprintf(stderr, "Error: insufficient argument for %s\n", ai);
				return 1;
			}
			period_sec = atof(argv[++i]);
		}
		else if (!strcmp(ai, "--guardband")) {
			if (i+1 >= argc) {
				fprintf(stderr, "Error: insufficient argument for %s\n", ai);
				return 1;
			}
			guardband_sec = atof(argv[++i]);
		}
		else if (!strcmp(ai, "--min-cycle")) {
			if (i+1 >= argc) {
				fprintf(stderr, "Error: insufficient argument for %s\n", ai);
				return 1;
			}
			min_cycle = atoi(argv[++i]);
		}
		else {
			fprintf(stderr, "Error: unknown argument %s\n", ai);
			return 1;
		}
	}
	if (!data) {
		data = DEFAULT_DATA;
	}

	mc.ctx = audio_context_create();
	mc.ctx->data = data;
	for (int i=0; data[i].freq > 0; i++) {
		double p = period_sec;
		if (p - guardband_sec < min_cycle / data[i].freq)
			p = min_cycle / data[i].freq + guardband_sec;

		const uint64_t period = 48000 * p;
		data[i].ts_begin_src = ts_end;
		data[i].ts_begin_cap = ts_end + 48000 * guardband_sec;
		data[i].ts_end = ts_end = ts_end + period;
	}
	mc.ctx->print_entry = print_entry;
	pa_context_set_state_callback(mc.ctx->pa, state_callback, &mc);

	int cnt = 1024*4;
	int retval = 0;
	while (pa_mainloop_iterate(mc.ctx->mainloop, 1024, &retval) >= 0) {
		if (mc.src && mc.cap) {
			if (capture_ts(mc.cap) >= ts_end)
				pa_mainloop_quit(mc.ctx->mainloop, 0);
		}
	}

	if (mc.src)
		source_release(mc.src);

	if (mc.cap)
		capture_release(mc.cap);

	audio_context_release(mc.ctx);
	return retval;
}
