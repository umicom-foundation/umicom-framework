/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_http_internal.h
 *
 * PURPOSE:
 *   Share one checked loopback HTTP policy across full and streaming provider transports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_LOCAL_HTTP_INTERNAL_H
#define UMICOM_AI_WORKSPACE_LOCAL_HTTP_INTERNAL_H
#include "umicom/base/status.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#include <curl/curl.h>
typedef struct AwLocalHttpCallbacks {
    curl_write_callback write;
    void *write_context;
    curl_write_callback header;
    void *header_context;
    curl_xferinfo_callback progress;
    void *progress_context;
} AwLocalHttpCallbacks;
/* Caller owns a newly initialized easy handle and libcurl global lifetime.
 * Body is an encoded, terminated JSON string borrowed until transfer cleanup;
 * NULL selects the fixed health GET. Header list and callback contexts are
 * borrowed for the same lifetime. No option accepts an arbitrary URL.
 * Every option is checked. A failure leaves a partially configured handle
 * that the caller must clean up without performing a transfer.
 * Streaming requires a chat body and header callback, selects HTTP/1.1,
 * requests identity encoding and disables entity decompression. Transfer
 * framing remains decoded by libcurl before payload callbacks. */
UmiStatus AwLocalHttpConfigure(CURL *curl,uint16_t port,uint32_t timeout_millis,
    const char *body,struct curl_slist *headers,const AwLocalHttpCallbacks *callbacks,bool streaming);
#endif
#endif
