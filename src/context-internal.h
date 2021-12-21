#pragma once

#include <stdbool.h>
#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/mainloop.h>
#include <pulse/stream.h>

struct audio_context_s
{
	volatile int refcnt;
	pa_mainloop *mainloop;
	pa_context *pa;
	pa_sample_spec spec;
	pa_stream *sync_stream;

	void (*print_entry)(const struct data_entry_t *entry, bool first);

	struct data_entry_t *data;
};

