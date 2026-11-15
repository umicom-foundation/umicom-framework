/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_stream_internal.h
 *
 * PURPOSE:
 *   Compose provider event framing and request-owned text behind a private terminal-sequence boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_CHAT_STREAM_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_CHAT_STREAM_INTERNAL_H
#include "local_chat_internal.h"
#include "umicom/ai/response_stream.h"
typedef struct AwLocalChatStream AwLocalChatStream;
typedef enum AwLocalChatStreamPhase {
    AW_LOCAL_CHAT_ACTIVE=0, AW_LOCAL_CHAT_FINISH_REPORTED=1,
    AW_LOCAL_CHAT_MARKER=2, AW_LOCAL_CHAT_COMPLETE=3,
    AW_LOCAL_CHAT_FAILED=4, AW_LOCAL_CHAT_CANCELLED=5
} AwLocalChatStreamPhase;
typedef struct AwLocalChatStreamSnapshot {
    AwLocalChatStreamPhase phase;
    UmiStatus failure, terminal_publication_status;
    UmiAiResponseStreamSnapshot text;
    char completion_id[UMI_AI_ID_CAPACITY];
    UmiAiFinishReason finish_reason;
    UmiAiUsage usage;
    bool has_usage;
} AwLocalChatStreamSnapshot;
/* Private owner-thread service. Serialize all calls and join the producer
 * before Destroy. Only request/model identity is copied, never message text.
 * Local identity remains distinct from the untrusted provider completion ID.
 * Output unchanged on failure; optional JSON capability must be available. */
UmiStatus AwLocalChatStreamCreate(const UmiAiRequest *request,
    UmiAiResponseStreamIdentity identity,size_t text_limit,uint64_t wire_limit,
    AwLocalChatStream **out);
void AwLocalChatStreamDestroy(AwLocalChatStream *stream);
/* Process all input, including multiple events. Earlier complete text events
 * may already be retained when a later event fails. Failures are sticky.
 * Empty input does not finish. Only default "message" SSE events are admitted.
 * After DONE, only ASCII space, tab, CR and LF are accepted until Finish. */
UmiStatus AwLocalChatStreamFeed(AwLocalChatStream *stream,const void *bytes,size_t length);
/* Call only after successful HTTP completion and validated response headers.
 * Requires a finish chunk then DONE. The parser cannot infer transport success
 * from bytes. Completion is published here, never when DONE alone arrives. */
UmiStatus AwLocalChatStreamFinish(AwLocalChatStream *stream);
/* Cancel retains accepted text; it does not interrupt HTTP or join a worker. */
UmiStatus AwLocalChatStreamCancel(AwLocalChatStream *stream);
/* Record a transport failure without interpreting EOF as completion. OK, BUSY
 * and CANCELLED are not failure inputs; use Cancel for cancellation. */
UmiStatus AwLocalChatStreamFail(AwLocalChatStream *stream,UmiStatus failure);
/* Copy text and matching owner-thread protocol metadata. Caller releases text
 * with UmiAiResponseStreamTextFree. Both outputs remain unchanged on failure.
 * Consult phase/failure and text.state; a synchronization failure while marking
 * terminal state is recorded separately in terminal_publication_status. */
UmiStatus AwLocalChatStreamCopy(AwLocalChatStream *stream,char **out_text,
    AwLocalChatStreamSnapshot *out_snapshot);
/* Read only synchronized text while the producer runs. This touches no mutable
 * protocol metadata. The stream owner must remain alive until readers return. */
UmiStatus AwLocalChatStreamTextCopy(AwLocalChatStream *stream,char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot);
/* Protocol inspection is serialized with the producer, or after its completion
 * has been synchronized. It avoids allocating a second complete text copy. */
UmiStatus AwLocalChatStreamInspect(AwLocalChatStream *stream,
    AwLocalChatStreamSnapshot *out_snapshot);
/* Bounded text-only observation follows UmiAiResponseStreamCopyRange.
 * It may run beside the producer while the protocol and text owners stay alive;
 * it does not inspect mutable protocol metadata. */
UmiStatus AwLocalChatStreamTextCopyRange(AwLocalChatStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot);
#endif
