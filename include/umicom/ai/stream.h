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

/**
 * Represent the ai stream collector data shared with callers of this public contract.
 */
typedef struct UmiAiStreamCollector {
    char text[UMI_AI_TEXT_CAPACITY * 4U];
    size_t length;
    UmiAiStreamSink sink;
    void *user_data;
} UmiAiStreamCollector;

/**
 * Initialise ai stream from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_stream_init(UmiAiStreamCollector *stream,
                        UmiAiStreamSink sink,
                        void *user_data);
/**
 * Provide the ai stream push operation used by this module and its client applications.
 */
/* The legacy collector accepts NUL-terminated bytes, not necessarily UTF-8.
 * Input must be readable through its NUL or the remaining collector capacity.
 * Corrupt stored length/text is INVALID_STATE; insufficient room leaves all
 * state unchanged. Input may alias text. Sink text is borrowed only during the
 * call and must not be retained. final_chunk is forwarded unchanged; this
 * compatibility API does not enforce final-once or request identity. */
UmiStatus umi_ai_stream_push(UmiAiStreamCollector *stream,
                             const char *text,
                             int final_chunk);

#ifdef __cplusplus
}
#endif

#endif
