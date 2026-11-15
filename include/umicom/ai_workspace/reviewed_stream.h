/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/reviewed_stream.h
 *
 * PURPOSE:
 *   Compose reviewed workspace jobs with native local streaming and durable final outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_REVIEWED_STREAM_H
#define UMICOM_AI_WORKSPACE_REVIEWED_STREAM_H
#include "umicom/ai_workspace/local_stream.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiWorkspaceStream UmiAiWorkspaceStream;
typedef struct UmiAiWorkspaceStreamOptions {
    uint64_t maximum_wire_bytes;
    bool allow_network;
} UmiAiWorkspaceStreamOptions;
typedef enum UmiAiWorkspaceStreamPhase {
    UMI_AI_WORKSPACE_STREAM_PREPARED,
    UMI_AI_WORKSPACE_STREAM_RUNNING,
    UMI_AI_WORKSPACE_STREAM_SETTLED
} UmiAiWorkspaceStreamPhase;
typedef struct UmiAiWorkspaceStreamSnapshot {
    UmiAiWorkspaceStreamPhase phase;
    UmiAiLocalStreamSnapshot transport;
    bool cancellation_requested;
    bool worker_started;
} UmiAiWorkspaceStreamSnapshot;
typedef struct UmiAiWorkspaceStreamResult {
    UmiStatus operation_status;
    UmiStatus publication_status;
    UmiAiLocalStreamResult transport;
    bool transport_result_available;
    bool worker_started;
} UmiAiWorkspaceStreamResult;
/* All calls belong to the transport's creating thread. The workspace and its
 * borrowed runtime/Data Server remain serialized there. Their storage and the
 * registered provider's borrowed cancellation token must outlive this owner.
 * Create accepts only an APPROVED draft/grounded job and the canonical local
 * provider. It copies the request and endpoint settings, retains workspace and
 * transport, but writes no RUNNING record and starts no worker/HTTP.
 * Unsupported providers/tools return NOT_IMPLEMENTED. Already successful jobs
 * return ALREADY_EXISTS. Failure leaves the output unchanged.
 * Network admission must be explicit. Text is bounded to the existing saved
 * response capacity; wire limit is 1..UMI_AI_LOCAL_STREAM_MAX_WIRE_BYTES. */
UmiStatus UmiAiWorkspaceStreamCreate(UmiAiWorkspace *workspace, const char *job_id,
    UmiAiLocalTransport *transport, const UmiAiWorkspaceStreamOptions *options,
    UmiAiWorkspaceStream **out_stream);
/* Recheck policy and canonical endpoint binding, durably publish RUNNING, then
 * start the worker. A failure before durable start leaves PREPARED. A failure
 * afterwards settles a failed/cancelled job; retain the owner and inspect Result.
 * No automatic retry occurs after durable start. */
UmiStatus UmiAiWorkspaceStreamStart(UmiAiWorkspaceStream *stream);
/* Before Start, cancellation abandons this prepared transport; Destroy leaves
 * the approved job unchanged. After Start it requests cooperative cancellation.
 * Cancellation never frees storage or proves native joining. The registered
 * provider token is forwarded during Start, Poll and Finish entry. A blocking
 * Finish cannot forward a later token change while waiting; call Cancel before
 * blocking Finish when closing, or keep polling/TryFinish in an event loop. */
UmiStatus UmiAiWorkspaceStreamCancel(UmiAiWorkspaceStream *stream);
UmiStatus UmiAiWorkspaceStreamPoll(UmiAiWorkspaceStream *stream,
    UmiAiWorkspaceStreamSnapshot *out_snapshot);
UmiStatus UmiAiWorkspaceStreamCopyText(UmiAiWorkspaceStream *stream, char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot);
/* TryFinish returns BUSY until native joining succeeds; unsupported platforms
 * return NOT_IMPLEMENTED. Finish is the explicitly blocking alternative.
 * Both then validate and attempt durable final publication exactly once.
 * OK means settlement is observable, not that generation/publication succeeded.
 * Inspect Result's separate statuses. Storage publication may itself take time.
 * A SETTLED owner is harmless to finish again; PREPARED is INVALID_STATE. */
UmiStatus UmiAiWorkspaceStreamTryFinish(UmiAiWorkspaceStream *stream);
UmiStatus UmiAiWorkspaceStreamFinish(UmiAiWorkspaceStream *stream);
UmiStatus UmiAiWorkspaceStreamReadResult(UmiAiWorkspaceStream *stream,
    UmiAiWorkspaceStreamResult *out_result);
/* Refuse RUNNING, including transport-finished work not yet settled. Native
 * handle-release failure retains the owner for retry. A successful release
 * clears *stream; text copies remain separately owned by the caller. */
UmiStatus UmiAiWorkspaceStreamDestroy(UmiAiWorkspaceStream **in_out_stream);
/* Bounded copied text follows UmiAiResponseStreamCopyRange semantics.
 * Keep this controller alive and use its existing owner-thread rules. */
UmiStatus UmiAiWorkspaceStreamCopyTextRange(UmiAiWorkspaceStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
