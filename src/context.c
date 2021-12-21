#include <stdlib.h>
#include <stdio.h>

#include "context.h"
#include "context-internal.h"

static pa_mainloop *mainloop = NULL;

audio_context_t *audio_context_create()
{
	audio_context_t *ctx = calloc(1, sizeof(audio_context_t));
	int ret;

	ctx->mainloop = pa_mainloop_new();
	ctx->pa = pa_context_new(
			pa_mainloop_get_api(ctx->mainloop),
			"audio-network-analyzer" );
	if (!ctx->pa) {
		fprintf(stderr, "Error: pa_context_new returns NULL\n");
		goto err;
	}

	ret = pa_context_connect(ctx->pa, NULL, 0, NULL);
	if (ret<0) {
		fprintf(stderr, "Error: pa_context_connect returns %d\n", ret);
		goto err;
	}

	ctx->spec.format = PA_SAMPLE_S16NE;
	ctx->spec.channels = 2;
	ctx->spec.rate = 48000;

	return ctx;

err:
	if (ctx->pa)
		pa_context_unref(ctx->pa);
	free(ctx);
	return NULL;
}

void audio_context_addref(audio_context_t *ctx)
{
	ctx->refcnt++;
}

static void audio_context_destroy(audio_context_t *ctx)
{
	pa_context_disconnect(ctx->pa);

	pa_context_unref(ctx->pa);

	pa_mainloop_free(ctx->mainloop);

	free(ctx);
}

void audio_context_release(audio_context_t *ctx)
{
	if (ctx->refcnt)
		ctx->refcnt--;
	else
		audio_context_destroy(ctx);
}
