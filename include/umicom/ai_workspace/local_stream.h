/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/local_stream.h
 *
 * PURPOSE:
 *   Expose request-owned background local model streams without borrowing application or GUI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_STREAM_H
#define UMICOM_AI_WORKSPACE_LOCAL_STREAM_H
#include <stdbool.h>
#include "umicom/ai_workspace/local_transport.h"
#include "umicom/ai_workspace/workspace.h"
#include "umicom/ai/response_stream.h"
#include "umicom/web/sse_response.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_LOCAL_STREAM_MAX_WIRE_BYTES (256ULL*1024ULL*1024ULL)
typedef struct UmiAiLocalStream UmiAiLocalStream;
typedef struct UmiAiLocalStreamOptions {
    uint16_t port;
    uint32_t timeout_millis;
    size_t maximum_text_bytes;
    uint64_t maximum_wire_bytes;
    UmiAiResponseStreamIdentity identity;
    bool allow_network; /* Explicit caller admission, not a consent dialog. */
} UmiAiLocalStreamOptions;
typedef enum UmiAiLocalStreamState {
    UMI_AI_LOCAL_STREAM_CREATED=0, UMI_AI_LOCAL_STREAM_RUNNING=1,
    UMI_AI_LOCAL_STREAM_FINISHED=2, UMI_AI_LOCAL_STREAM_JOINED=3
} UmiAiLocalStreamState;
typedef struct UmiAiLocalStreamSnapshot {
    UmiAiLocalStreamState state;
    UmiStatus operation_status; /* BUSY until the operation finishes. */
    bool cancellation_requested;
} UmiAiLocalStreamSnapshot;
typedef struct UmiAiLocalStreamResult {
    UmiStatus operation_status;
    UmiStatus observation_status; /* Final protocol snapshot status. */
    UmiStatus terminal_publication_status;
    UmiWebSseResponseSnapshot response;
    UmiAiResponseStreamSnapshot text;
    char completion_id[UMI_AI_ID_CAPACITY];
    UmiAiFinishReason finish_reason;
    UmiAiUsage usage;
    bool has_usage;
} UmiAiLocalStreamResult;
/* All controller calls belong to the runtime's creating thread and must be
 * serialized there. Create validates and encodes the request, copies identity
 * and options, allocates bounded text, and retains transport. It starts neither
 * a thread nor HTTP. Request/options may be released or edited after return.
 * No credentials, remote URL, callback or GUI pointer is stored.
 * Port 1024..65535; timeout 100..60000; text 1..AI_RESPONSE_STREAM_MAX_BYTES;
 * wire 1..LOCAL_STREAM_MAX_WIRE_BYTES; local identity tokens must be nonzero.
 * allow_network must be true. Output pointers remain unchanged on failure. */
UmiStatus UmiAiLocalStreamCreate(UmiAiLocalTransport *transport,
    const UmiAiRequest *request,const UmiAiLocalStreamOptions *options,
    UmiAiLocalStream **out_stream);
/* Start once. A thread-start failure leaves CREATED and may be retried.
 * Cancelling a CREATED job finishes it without creating a worker. */
UmiStatus UmiAiLocalStreamStart(UmiAiLocalStream *stream);
UmiStatus UmiAiLocalStreamCancel(UmiAiLocalStream *stream);
/* FINISHED means the operation published its result, not that native thread
 * teardown has ended. Neither FINISHED nor a terminal text snapshot is a join.
 * Poll/CopyText do not wait for the HTTP operation to finish. CopyText uses
 * the bounded text owner's mutex and returns a caller-owned UTF-8 string. */
UmiStatus UmiAiLocalStreamPoll(UmiAiLocalStream *stream,UmiAiLocalStreamSnapshot *out);
UmiStatus UmiAiLocalStreamCopyText(UmiAiLocalStream *stream,char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot);
/* BUSY until operation publication. Inspect operation_status and
 * observation_status separately. Final response/protocol metadata is immutable.
 * Completion IDs and reported model fields are not authenticated identities. */
UmiStatus UmiAiLocalStreamReadResult(UmiAiLocalStream *stream,UmiAiLocalStreamResult *out);
/* TryJoin is genuinely nonblocking only on supported Framework platforms;
 * unsupported builds return NOT_IMPLEMENTED. Join may block until the bounded
 * HTTP operation and native teardown finish. Cancel first when closing.
 * Both are harmless after a confirmed join or when no worker was started. */
UmiStatus UmiAiLocalStreamTryJoin(UmiAiLocalStream *stream);
UmiStatus UmiAiLocalStreamJoin(UmiAiLocalStream *stream);
/* BUSY while a started native worker has not been joined, even after FINISHED.
 * Never implicitly cancels, blocks, detaches or frees worker storage.
 * Native handle-release failure retains *stream for retry. Successful release
 * clears it and drops the runtime reference. An empty *stream is harmless.
 * Free text copies separately with UmiAiResponseStreamTextFree. */
UmiStatus UmiAiLocalStreamDestroy(UmiAiLocalStream **in_out_stream);
/* Bounded copied text follows UmiAiResponseStreamCopyRange semantics.
 * Keep this controller alive and use its existing owner-thread rules. */
UmiStatus UmiAiLocalStreamCopyTextRange(UmiAiLocalStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
