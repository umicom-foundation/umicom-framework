/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_http_stream_internal.h
 *
 * PURPOSE:
 *   Define the private blocking exchange between local HTTP transport and a fresh model-stream owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_HTTP_STREAM_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_HTTP_STREAM_INTERNAL_H
#include "local_chat_stream_internal.h"
#include "umicom/web/sse_response.h"
typedef struct AwLocalHttpStreamResult {
    UmiStatus status;
    UmiWebSseResponseSnapshot response;
} AwLocalHttpStreamResult;
/* Private serialized transport. Caller owns initialized libcurl global state,
 * an encoded request from AwLocalChatEncodeStreaming and a fresh sequencer.
 * All borrowed values remain alive until return. No worker or callback into
 * application/UI code is created. Do not concurrently inspect or mutate the
 * sequencer; a future public worker owner must synchronize its observations.
 *
 * Port 1024..65535; timeout 100..60000 ms; positive wire budget. Only fixed
 * loopback POST is possible. HTTP/1 event-stream admission precedes payload.
 * Preconditions/capability failure leave out_result and sequencer unchanged.
 * Once admitted, setup/transport failures terminate the sequencer and publish
 * result metadata. Accepted text is retained on failure or cancellation.
 * A result status is not a model-quality or endpoint-ownership attestation. */
UmiStatus AwLocalChatExchangeStream(uint16_t port,uint32_t timeout_millis,
    const char *encoded_request,uint64_t wire_limit,
    const UmiAiWorkspaceCancellation *cancellation,AwLocalChatStream *stream,
    AwLocalHttpStreamResult *out_result);
#endif
