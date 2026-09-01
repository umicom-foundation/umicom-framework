/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/stream.h
 *
 * PURPOSE:
 *   Collect or forward incremental model-output chunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Streaming keeps the UI responsive while still allowing tests to collect the exact final text deterministically.
 */

#ifndef INCLUDE_UMICOM_AI_STREAM_H
#define INCLUDE_UMICOM_AI_STREAM_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/ai/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*UmiAiStreamSink)(const char *text, int final_chunk, void *user_data);

typedef struct UmiAiStreamCollector {
    char text[UMI_AI_TEXT_CAPACITY * 4U];
    size_t length;
    UmiAiStreamSink sink;
    void *user_data;
} UmiAiStreamCollector;

void umi_ai_stream_init(UmiAiStreamCollector *stream,
                        UmiAiStreamSink sink,
                        void *user_data);
UmiStatus umi_ai_stream_push(UmiAiStreamCollector *stream,
                             const char *text,
                             int final_chunk);

#ifdef __cplusplus
}
#endif

#endif
