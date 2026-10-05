/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/connection.c
 * PURPOSE: Run one bounded request through the existing service without coupling handlers to sockets or a GUI toolkit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/connection.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus WriteResponse(const UmiWebConnectionIo *io, const unsigned char *bytes, size_t length,
                               const UmiCancellationToken *cancel, UmiWebExchangeResult *result)
{
    size_t position = 0U;
    while (position < length)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        size_t written = 0U;
        UmiStatus status = io->write(io->context, bytes + position, length - position, &written);
        if (written > length - position)
            return UMI_STATUS_INVALID_STATE;
        position += written;
        result->bytes_sent += written;
        if (status != UMI_STATUS_OK)
            return status;
        if (written == 0U)
            return UMI_STATUS_IO_ERROR;
    }
    result->response_complete = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebConnectionProcess(UmiWebService *service, const UmiWebConnectionIo *transport, size_t maximum,
                                  const UmiCancellationToken *cancel, UmiWebExchangeResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (service == NULL || transport == NULL || transport->read == NULL || transport->write == NULL ||
        maximum == 0U || maximum > UMI_WEB_CONNECTION_REQUEST_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
    {
        out->request_status = UMI_STATUS_CANCELLED;
        return UMI_STATUS_CANCELLED;
    }
    UmiWebConnectionIo io = *transport;
    unsigned char *input = malloc(maximum), *output = malloc(UMI_WEB_CONNECTION_RESPONSE_LIMIT);
    UmiWebRequest *request = calloc(1U, sizeof(*request));
    UmiWebResponse *response = calloc(1U, sizeof(*response));
    UmiStatus status = UMI_STATUS_OK, operation = UMI_STATUS_OK;
    size_t received = 0U, wire_bytes = 0U;
    UmiWebRequestFrame frame;
    if (input == NULL || output == NULL || request == NULL || response == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto finished;
    }
    for (;;)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        size_t count = 0U;
        status = io.read(io.context, input + received, maximum - received, &count);
        if (count > maximum - received)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        received += count;
        out->bytes_received = received;
        if (status != UMI_STATUS_OK)
            break;
        if (count == 0U)
        {
            status = UMI_STATUS_UNAVAILABLE;
            break;
        }
        status = UmiWebRequestFrameRead(input, received, maximum, &frame);
        if (status != UMI_STATUS_BUSY)
            break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiWebRequestParseStrict(input, received, maximum, request);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    out->request_status = status;
    operation = status;
    if (status == UMI_STATUS_OK)
    {
        out->dispatched = true;
        status = umi_web_service_handle(service, request, response);
        out->dispatch_status = status;
        operation = status;
        if (status != UMI_STATUS_OK)
            status = umi_web_response_set_text(response, 500, "text/plain; charset=utf-8",
                                               "The request handler failed.\n");
    }
    else if (status == UMI_STATUS_PARSE_ERROR || status == UMI_STATUS_CAPACITY_EXCEEDED ||
             status == UMI_STATUS_NOT_IMPLEMENTED)
    {
        int code = status == UMI_STATUS_CAPACITY_EXCEEDED ? 413
                   : status == UMI_STATUS_NOT_IMPLEMENTED ? 501
                                                          : 400;
        status = umi_web_response_set_text(response, code, "text/plain; charset=utf-8",
                                           "This request cannot be processed.\n");
    }
    else
        goto finished;
    if (status != UMI_STATUS_OK)
        goto finished;
    status = UmiWebResponseFormatClosed(response, request->method == UMI_HTTP_METHOD_HEAD, output,
                                        UMI_WEB_CONNECTION_RESPONSE_LIMIT, &wire_bytes);
    if (status != UMI_STATUS_OK)
    {
        /* A malformed handler response cannot inject wire headers. Send a
         * known replacement, preserving the failure for the caller. */
        operation = status;
        out->dispatch_status = status;
        status = umi_web_response_set_text(response, 500, "text/plain; charset=utf-8",
                                           "The response could not be encoded.\n");
        if (status == UMI_STATUS_OK)
            status = UmiWebResponseFormatClosed(response, request->method == UMI_HTTP_METHOD_HEAD, output,
                                                UMI_WEB_CONNECTION_RESPONSE_LIMIT, &wire_bytes);
    }
    if (status == UMI_STATUS_OK)
    {
        out->response_status = response->status;
        status = WriteResponse(&io, output, wire_bytes, cancel, out);
        if (status == UMI_STATUS_OK)
            status = operation;
    }
finished:
    if (!out->dispatched && out->request_status == UMI_STATUS_OK && status != UMI_STATUS_OK)
        out->request_status = status;
    free(input);
    free(output);
    free(request);
    free(response);
    return status;
}
