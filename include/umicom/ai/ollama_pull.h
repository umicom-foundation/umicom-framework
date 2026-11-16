/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/ollama_pull.h
 *
 * PURPOSE:
 *   Decode copied Ollama pull observations and frame bounded progress streams for native SDK clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_OLLAMA_PULL_H
#define UMICOM_AI_OLLAMA_PULL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OLLAMA_PULL_LINE_BYTES 16384U
#define UMI_OLLAMA_PULL_STREAM_BYTES (16U * 1024U * 1024U)
#define UMI_OLLAMA_PULL_MAX_EVENTS 65536U
#define UMI_OLLAMA_PULL_TOTAL_KNOWN 1U
#define UMI_OLLAMA_PULL_COMPLETED_KNOWN 2U
typedef enum UmiOllamaPullEventKind {
    UMI_OLLAMA_PULL_STATUS = 1,
    UMI_OLLAMA_PULL_SUCCESS = 2,
    UMI_OLLAMA_PULL_ERROR = 3
} UmiOllamaPullEventKind;
typedef struct UmiOllamaPullEvent {
    UmiOllamaPullEventKind kind;
    char status[257], digest[129], error[1025];
    uint64_t total, completed;
    uint32_t known;
} UmiOllamaPullEvent;
/* Decode exactly one JSON object, not a stream or HTTP response. Require a
 * nonempty status or error; error takes precedence if both are present. Exact
 * status "success" identifies a provider terminal claim. Optional text must be
 * nonempty when present. Counters require integer spelling 0..INT64_MAX; when
 * both are present completed <= total. Missing numbers retain clear known bits.
 * Unknown fields are validated and ignored; all decoded object keys are unique.
 * Limits: 16 KiB, 512 nodes, 16 containers. Output is unchanged on every failure.
 * Digest text is opaque unverified metadata. No file or network operation occurs.
 * Input/token are borrowed for this call; output is an independent copy. */
UmiStatus UmiOllamaPullEventDecode(const void *bytes,size_t length,
    const UmiCancellationToken *cancel,UmiOllamaPullEvent *out);

typedef struct UmiOllamaPullStream UmiOllamaPullStream;
typedef struct UmiOllamaPullSnapshot {
    size_t bytes_admitted, events, pending_bytes;
    int has_event, terminal_seen, input_finished;
    UmiStatus failure;
    UmiOllamaPullEvent last;
} UmiOllamaPullSnapshot;
/* Callback receives a borrowed event only for its duration. Copy it to retain it.
 * It may Read the stream, but Feed/Finish/Release during a callback return BUSY.
 * A non-OK callback result latches that failure; the delivered event remains
 * counted. No callback is undone or retried. Keep callback context alive until
 * Release. A NULL callback is allowed for last-observation-only consumers. */
typedef UmiStatus (*UmiOllamaPullObserver)(const UmiOllamaPullEvent *event,void *context);
UmiStatus UmiOllamaPullStreamCreate(UmiOllamaPullObserver observer,void *context,
    UmiOllamaPullStream **out);
/* All calls must be serialized; this is not a thread synchronization primitive.
 * Feed accepts arbitrary byte splits, LF or CRLF, and ignores empty lines only.
 * A final line without LF is decoded by Finish. A bare CR is not a separator.
 * Length excludes LF but includes a possible trailing CR for the 16 KiB limit.
 * Each admitted chunk counts in full toward 16 MiB, even if it later fails.
 * Limit 65536 nonempty records. Invalid arguments and BUSY do not mutate state.
 * After admission, parse/bound/callback/cancellation failures latch and discard
 * pending text. Already delivered records remain observable. No retry/reset.
 * Records after a success/error marker are refused; trailing empty lines work.
 * Feed returning OK is not completion. An error event is delivered with OK;
 * Finish converts a provider error to IO_ERROR. Finish requires a terminal
 * marker, observes late cancellation, and is idempotent after completion.
 * Call Finish only after transport EOF with an independently successful HTTP
 * outcome. On transport failure release the owner without claiming success.
 * Success here validates a reported protocol sequence, not downloaded bytes,
 * model execution, endpoint identity or permission to perform a pull.
 * NULL bytes is valid only for a zero-length Feed. Inputs remain borrowed for
 * the call; no callback may modify them or destroy the cancellation token. */
UmiStatus UmiOllamaPullStreamFeed(UmiOllamaPullStream *stream,const void *bytes,
    size_t length,const UmiCancellationToken *cancel);
UmiStatus UmiOllamaPullStreamFinish(UmiOllamaPullStream *stream,
    const UmiCancellationToken *cancel);
UmiStatus UmiOllamaPullStreamRead(const UmiOllamaPullStream *stream,UmiOllamaPullSnapshot *out);
/* Release clears owned response text. NULL slot is invalid; an empty slot is OK.
 * BUSY preserves the owner; otherwise clear/free it and set the slot to NULL. */
UmiStatus UmiOllamaPullStreamRelease(UmiOllamaPullStream **owner);
#ifdef __cplusplus
}
#endif
#endif
