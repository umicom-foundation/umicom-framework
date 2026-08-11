/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/stream.c
 *
 * PURPOSE:
 *   Append incremental model-output chunks and notify an optional sink.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The collector separates transport chunking from application state so GTK4, CLI and headless tests can consume the same stream.
 */

#include "umicom/ai/stream.h"
#include <stddef.h>

#include <string.h>

void umi_ai_stream_init(UmiAiStreamCollector *stream,
                        UmiAiStreamSink sink,
                        void *user_data)
{
    if (stream != NULL) {
        stream->text[0] = '\0';
        stream->length = 0U;
        stream->sink = sink;
        stream->user_data = user_data;
    }
}

UmiStatus umi_ai_stream_push(UmiAiStreamCollector *stream,
                             const char *text,
                             int final_chunk)
{
    size_t length;
    if (stream == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (stream->length + length + 1U > sizeof(stream->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(stream->text + stream->length, text, length + 1U);
    stream->length += length;
    if (stream->sink != NULL) {
        stream->sink(text, final_chunk, stream->user_data);
    }
    return UMI_STATUS_OK;
}
