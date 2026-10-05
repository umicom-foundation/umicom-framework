/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/connection.h
 * PURPOSE: Process one bounded HTTP exchange through reusable C routing and an explicit byte transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_CONNECTION_H
#define UMICOM_WEB_CONNECTION_H
#include "umicom/web/service.h"
#include "umicom/platform/cancellation.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_WEB_CONNECTION_REQUEST_LIMIT (64U * 1024U)
#define UMI_WEB_CONNECTION_RESPONSE_LIMIT (64U * 1024U)
    typedef struct UmiWebRequestFrame
    {
        size_t header_bytes, body_bytes, message_bytes;
    } UmiWebRequestFrame;
    /* Inspect a complete or partial HTTP/1.1 origin-form request. BUSY means more
 * bytes are needed; malformed framing never dispatches a handler. Require one
 * nonempty Host and at most one decimal Content-Length. Transfer-Encoding,
 * Expect and Upgrade are not implemented; duplicate lengths, line folding,
 * control bytes in headers and extra pipelined bytes are refused. The body may
 * contain NUL. This bounded local-development subset is not a full HTTP server.
 * maximum_bytes is 1..REQUEST_LIMIT; body must fit the existing request type.
 * Output is cleared on failure or incomplete input. No I/O or allocation. */
    UmiStatus UmiWebRequestFrameRead(const void *bytes, size_t length, size_t maximum_bytes,
                                     UmiWebRequestFrame *out);
    /* Parse exactly one validated frame, trimming header value whitespace.
 * Failure leaves the destination unchanged. Large request values belong on
 * the heap when called from a thread with a small stack. */
    UmiStatus UmiWebRequestParseStrict(const void *bytes, size_t length, size_t maximum_bytes,
                                       UmiWebRequest *out);
    /* Validate a response and serialize authoritative Content-Length and
 * Connection: close. Caller-supplied framing headers are refused. HEAD omits
 * the body while retaining its length; 204/304 omit body and length. No
 * caller output bytes change on failure; out_length is cleared on entry. */
    UmiStatus UmiWebResponseFormatClosed(const UmiWebResponse *response, bool head_request, void *out_bytes,
                                         size_t capacity, size_t *out_length);
    typedef struct UmiWebConnectionIo
    {
        void *context;
        /* Callbacks report 1..capacity bytes on success. A peer close is
     * UNAVAILABLE; timeout/cancel are explicit. They must bound their waits.
     * Process borrows the descriptor/context and never closes the transport. */
        UmiStatus (*read)(void *context, void *bytes, size_t capacity, size_t *out_bytes);
        UmiStatus (*write)(void *context, const void *bytes, size_t length, size_t *out_bytes);
    } UmiWebConnectionIo;
    typedef struct UmiWebExchangeResult
    {
        size_t bytes_received, bytes_sent;
        /* dispatched means service routing ran, including a 404 with no
         * matching application callback. It does not prove client receipt. */
        bool dispatched, response_complete;
        int response_status;
        UmiStatus request_status, dispatch_status;
    } UmiWebExchangeResult;
    /* One owner thread; the service and callback context must outlive return.
 * Read one bounded request, invoke the existing service once, write one
 * complete response. No retry, persistence, file access or implicit endpoint.
 * Failures may still send a 400/413/501/500 response; return the original
 * request/handler failure after sending it. A failed send does not imply the
 * handler had no effect. Inspect dispatched and response_complete.
 * Cancellation is cooperative between bounded callbacks and before dispatch;
 * a synchronous handler must return before cancellation can be observed.
 * The host must close the connection after return, including all failures. */
    UmiStatus UmiWebConnectionProcess(UmiWebService *service, const UmiWebConnectionIo *io,
                                      size_t maximum_request_bytes, const UmiCancellationToken *cancel,
                                      UmiWebExchangeResult *out);
#ifdef __cplusplus
}
#endif
#endif
