/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/response_stream.h
 *
 * PURPOSE:
 *   Own bounded incremental model text with request identity, terminal state and copied publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_RESPONSE_STREAM_H
#define UMICOM_AI_RESPONSE_STREAM_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_RESPONSE_STREAM_MAX_BYTES (64U*1024U*1024U)
typedef struct UmiAiResponseStream UmiAiResponseStream;
/* Nonzero local admission tokens, not provider string IDs or authenticated
 * model identities. The application assigns them and never reuses a retired
 * pair while old events can arrive. Neither token changes during this owner. */
typedef struct UmiAiResponseStreamIdentity {
    uint64_t request_id;
    uint64_t model_revision;
} UmiAiResponseStreamIdentity;
typedef enum UmiAiResponseStreamState {
    UMI_AI_RESPONSE_STREAM_ACTIVE=1,
    UMI_AI_RESPONSE_STREAM_COMPLETE=2,
    UMI_AI_RESPONSE_STREAM_CANCELLED=3,
    UMI_AI_RESPONSE_STREAM_FAILED=4
} UmiAiResponseStreamState;
typedef struct UmiAiResponseStreamSnapshot {
    UmiAiResponseStreamIdentity identity;
    UmiAiResponseStreamState state;
    UmiStatus status;
    uint64_t last_sequence;
    uint64_t terminal_sequence;
    size_t accepted_bytes;
    size_t text_bytes;
    size_t pending_bytes;
    size_t maximum_bytes;
} UmiAiResponseStreamSnapshot;

/* Allocate one fixed-capacity owner. Budget counts payload bytes, including a
 * partial scalar, and must be 1..MAX_BYTES. No worker, transport or callback is
 * started. Failure preserves *out_stream. */
UmiStatus UmiAiResponseStreamCreate(UmiAiResponseStreamIdentity identity,
    size_t maximum_bytes,UmiAiResponseStream **out_stream);
/* The caller must stop and join all producers/readers before destruction.
 * Cancel is not a join. A pointer borrowed by a worker cannot outlive its owner.
 * NULL is harmless; other operations may run concurrently while the owner lives. */
void UmiAiResponseStreamDestroy(UmiAiResponseStream *stream);
/* Submit the next event, starting at sequence 1. A wrong identity is NOT_FOUND;
 * a duplicate, gap, zero sequence or retired stream is INVALID_STATE, without
 * changing state. UINT64_MAX is reserved for a terminal event; a nonfinal
 * event at that sequence returns CAPACITY_EXCEEDED without mutation.
 * final_chunk must be 0 or 1. NULL bytes are allowed for size 0.
 * The input span stays readable and unchanged until return.
 *
 * UTF-8 is validated through Base. A complete input chunk is accepted atomically;
 * complete scalars become visible while at most three trailing bytes remain
 * pending. Embedded NUL or malformed/truncated-final UTF-8 terminates FAILED
 * with PARSE_ERROR. A budget overflow terminates FAILED with CAPACITY_EXCEEDED.
 * Neither failed chunk is appended; earlier accepted bytes/text are retained.
 * A final accepted event completes exactly once. Empty events are permitted.
 * No callback runs under the mutex. A native unlock failure can be reported
 * after a mutation was already committed; do not blindly retry its sequence. */
UmiStatus UmiAiResponseStreamPush(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity,uint64_t sequence,
    const unsigned char *bytes,size_t size,int final_chunk);
/* Cancel an active stream, retaining prior text/pending bytes. Repeated cancel
 * on that same cancelled stream succeeds. Completed/failed streams stay terminal.
 * Cancellation waits for an in-progress Push to release the mutex; use bounded
 * producer chunks when cancellation latency matters. It does not interrupt or
 * close an external provider/transport. */
UmiStatus UmiAiResponseStreamCancel(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity);
/* Record an external failure as the next event. Status must be a known non-OK,
 * non-CANCELLED Framework status. Earlier evidence is retained. */
UmiStatus UmiAiResponseStreamFail(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity,uint64_t sequence,UmiStatus status);
/* These observations require no identity because the owner itself is explicit.
 * Copy only complete UTF-8 text; output text and snapshot describe one locked
 * observation. A pending prefix is never exposed as a string. Caller frees the
 * copy with TextFree. Failure preserves caller outputs, which must be separate.
 * Copies remain valid after further pushes or destruction of the stream. */
UmiStatus UmiAiResponseStreamInspect(UmiAiResponseStream *stream,
    UmiAiResponseStreamSnapshot *out_snapshot);
UmiStatus UmiAiResponseStreamCopy(UmiAiResponseStream *stream,
    char **out_text,UmiAiResponseStreamSnapshot *out_snapshot);
/* Copy at most maximum_bytes payload bytes starting at a visible UTF-8 byte
 * offset. The offset must be a scalar boundary in 0..snapshot.text_bytes;
 * maximum_bytes must be 1..MAX_BYTES. At the visible end, return an owned empty
 * string even while a partial scalar is pending. If the first remaining scalar
 * cannot fit, return CAPACITY_EXCEEDED instead of an empty non-progressing read.
 * Snapshot describes the entire stream at the same locked observation; copied
 * is this range's byte count. Advance the caller's offset by copied only on OK.
 * No cursor is retained internally. Keep the same owner and inspect its identity;
 * offsets must not be reused for a different request. Terminal owners are readable.
 * All outputs are separate and unchanged on failure, including unlock failure.
 * Free the NUL-terminated copy with TextFree; it survives owner destruction.
 * This bounds copied allocation/work, not mutex wait time or producer activity. */
UmiStatus UmiAiResponseStreamCopyRange(UmiAiResponseStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot);
void UmiAiResponseStreamTextFree(char *text);
#ifdef __cplusplus
}
#endif
#endif
