/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/sse_response.h
 *
 * PURPOSE:
 *   Admit bounded event-stream response headers and entity bytes before payload publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_SSE_RESPONSE_H
#define UMICOM_WEB_SSE_RESPONSE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebSseResponse UmiWebSseResponse;
typedef enum UmiWebSseResponseState {
    UMI_WEB_SSE_RESPONSE_STATUS=0, UMI_WEB_SSE_RESPONSE_HEADERS=1,
    UMI_WEB_SSE_RESPONSE_READY=2, UMI_WEB_SSE_RESPONSE_BODY=3,
    UMI_WEB_SSE_RESPONSE_TRAILERS=4, UMI_WEB_SSE_RESPONSE_TRAILERS_DONE=5,
    UMI_WEB_SSE_RESPONSE_COMPLETE=6, UMI_WEB_SSE_RESPONSE_FAILED=7
} UmiWebSseResponseState;
typedef struct UmiWebSseResponseLimits {
    size_t line_bytes;       /* 2..65536, including CRLF. */
    size_t total_header_bytes; /* line_bytes..1048576, all blocks/trailers. */
    uint32_t header_lines;   /* 1..4096, including status and blank lines. */
    uint32_t informational_responses; /* 0..16; 101 is never supported. */
    uint64_t body_bytes;    /* Positive decoded HTTP entity-byte budget. */
} UmiWebSseResponseLimits;
typedef struct UmiWebSseResponseSnapshot {
    UmiWebSseResponseState state;
    UmiStatus failure;
    uint32_t http_status,informational_responses,header_lines;
    size_t header_bytes;
    uint64_t body_bytes,declared_body_bytes;
    bool has_content_length,chunked;
} UmiWebSseResponseSnapshot;
/* Narrow receiving profile for an HTTP/1.0 or HTTP/1.1 event-stream client.
 * Serialize calls on one owner thread. No socket, callback or body storage is
 * owned here. Output pointers are unchanged on failure. */
UmiStatus UmiWebSseResponseCreate(const UmiWebSseResponseLimits *limits,UmiWebSseResponse **out);
void UmiWebSseResponseDestroy(UmiWebSseResponse *response);
/* Feed exactly one complete status/header/blank line, including CRLF, as a
 * transport's header callback provides. Arbitrary network fragments must first
 * be assembled by the HTTP transport, not by the SSE payload parser.
 * Final status must be 200 and Content-Type exactly one text/event-stream,
 * optionally with one charset=utf-8 parameter (quoted or unquoted).
 * Only absent/identity Content-Encoding and absent/chunked Transfer-Encoding
 * are admitted. Content-Length and Transfer-Encoding cannot coexist.
 * Non-200 final/101 or unsupported type/encoding returns UNAVAILABLE.
 * Malformed or duplicate framing fields return PARSE_ERROR. Budget failures
 * return CAPACITY_EXCEEDED. These refusals are sticky.
 * Trailers are allowed after Body only with chunked framing, and cannot revise routing/framing/media
 * fields. Header bytes are not copied or retained. */
UmiStatus UmiWebSseResponseHeader(UmiWebSseResponse *response,const void *line,size_t bytes);
/* Admit entity bytes before forwarding them to a payload decoder. Transfer
 * framing has already been removed by the HTTP transport. Calls after trailers
 * fail. Zero bytes is permitted and begins body phase without inventing data.
 * Count is unchanged for a rejected body span; previous bytes stay observed. */
UmiStatus UmiWebSseResponseBody(UmiWebSseResponse *response,size_t bytes);
/* Only after successful transport completion. Checks any declared byte count.
 * Empty entity bodies can pass here; an SSE/model sequencer must independently
 * require its own terminal records. Repeated successful Finish is harmless. */
UmiStatus UmiWebSseResponseFinish(UmiWebSseResponse *response);
UmiStatus UmiWebSseResponseInspect(const UmiWebSseResponse *response,UmiWebSseResponseSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
