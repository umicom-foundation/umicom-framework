/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/fastcgi_exchange.c
 *
 * PURPOSE:
 *   Frame one bounded worker request, handle fragmented transport I/O and
 *   retain output only after verified FastCGI completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi_exchange.h"
#include <stdlib.h>
#include <string.h>

#define FASTCGI_RECEIVE_BYTES ((size_t)UMI_WEB_FASTCGI_CONTENT_LIMIT + 8U + UINT8_MAX)

static UmiStatus RequestCapacity(const UmiWebFastCgiRequest *request, size_t *capacity)
{
    if (request->requestId == 0U || request->replyLimit == 0U ||
        (request->variableCount != 0U && request->variables == NULL) ||
        (request->bodyBytes != 0U && request->body == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (request->variableCount > UMI_WEB_FASTCGI_MAX_PARAMETERS ||
        request->replyLimit > UMI_WEB_FASTCGI_EXCHANGE_LIMIT ||
        request->bodyBytes > UMI_WEB_FASTCGI_EXCHANGE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    const size_t chunk = UMI_WEB_FASTCGI_CONTENT_LIMIT;
    const size_t frames = request->bodyBytes / chunk + (request->bodyBytes % chunk != 0U);
    size_t total = 32U + request->bodyBytes + 16U * frames;
    for (size_t i = 0U; i < request->variableCount; ++i) {
        const UmiWebFastCgiVariable *item = &request->variables[i];
        if (item->name == NULL || item->nameBytes == 0U) return UMI_STATUS_INVALID_ARGUMENT;
        if (item->nameBytes > chunk || item->valueBytes > chunk) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t previous = 0U; previous < i; ++previous) {
            const UmiWebFastCgiVariable *other = &request->variables[previous];
            if (item->nameBytes == other->nameBytes && memcmp(item->name, other->name, item->nameBytes) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
        total += item->nameBytes + item->valueBytes + 24U;
    }
    /* The upper estimate includes padding. The encoder checks the exact wire
     * budget before any transfer, including a request close to the limit. */
    *capacity = total < UMI_WEB_FASTCGI_EXCHANGE_LIMIT ? total : UMI_WEB_FASTCGI_EXCHANGE_LIMIT;
    return UMI_STATUS_OK;
}

/* Prepare every record before a transport can have a side effect. This also
 * lets the caller choose different short-write behavior without reframing. */
static UmiStatus EncodeRequest(const UmiWebFastCgiRequest *request, uint8_t *wire,
    size_t capacity, uint8_t *scratch, size_t *written)
{
    size_t used = 0U, count = 0U;
    UmiStatus status = UmiWebFastCgiBegin(request->requestId, wire, capacity, &used);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < request->variableCount; ++i) {
        const UmiWebFastCgiVariable *item = &request->variables[i];
        size_t pair = 0U;
        status = UmiWebFastCgiParameter(item->name, item->nameBytes, item->value, item->valueBytes,
            scratch, FASTCGI_RECEIVE_BYTES, &pair);
        if (status != UMI_STATUS_OK) return status;
        status = UmiWebFastCgiInput(UMI_WEB_FASTCGI_PARAMS, request->requestId, scratch, pair,
            wire + used, capacity - used, &count);
        if (status != UMI_STATUS_OK) return status;
        used += count;
    }
    status = UmiWebFastCgiInput(UMI_WEB_FASTCGI_PARAMS, request->requestId, NULL, 0U,
        wire + used, capacity - used, &count);
    if (status != UMI_STATUS_OK) return status;
    used += count;

    const uint8_t *body = request->body;
    for (size_t offset = 0U; offset < request->bodyBytes;) {
        const size_t remaining = request->bodyBytes - offset;
        const size_t frame = remaining < UMI_WEB_FASTCGI_CONTENT_LIMIT ? remaining : UMI_WEB_FASTCGI_CONTENT_LIMIT;
        status = UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDIN, request->requestId, body + offset, frame,
            wire + used, capacity - used, &count);
        if (status != UMI_STATUS_OK) return status;
        used += count;
        offset += frame;
    }
    status = UmiWebFastCgiInput(UMI_WEB_FASTCGI_STDIN, request->requestId, NULL, 0U,
        wire + used, capacity - used, &count);
    if (status != UMI_STATUS_OK) return status;
    *written = used + count;
    return UMI_STATUS_OK;
}

static UmiStatus SendRequest(const UmiWebConnectionIo *io, const uint8_t *wire,
    size_t size, const UmiCancellationToken *cancel)
{
    for (size_t sent = 0U; sent < size;) {
        size_t count = 0U;
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        const UmiStatus status = io->write(io->context, wire + sent, size - sent, &count);
        if (status != UMI_STATUS_OK) return status;
        if (count == 0U || count > size - sent) return UMI_STATUS_IO_ERROR;
        sent += count;
    }
    return UMI_STATUS_OK;
}

static UmiStatus ReceiveReply(const UmiWebConnectionIo *io, UmiWebFastCgiReply *reply,
    uint8_t *frame, uint8_t *body, const UmiCancellationToken *cancel)
{
    size_t begin = 0U, end = 0U, wireBytes = 0U;
    while (!reply->complete) {
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        UmiWebFastCgiRecord record;
        const size_t before = reply->stdoutBytes;
        const UmiStatus status = UmiWebFastCgiReadReply(reply, frame + begin, end - begin, &record);
        if (status == UMI_STATUS_UNAVAILABLE) {
            size_t count = 0U;
            /* Advance within the frame buffer while complete records remain.
             * Move a partial record only when more transport bytes are needed. */
            if (begin != 0U) {
                end -= begin;
                memmove(frame, frame + begin, end);
                begin = 0U;
            }
            size_t space = FASTCGI_RECEIVE_BYTES - end;
            const size_t remaining = UMI_WEB_FASTCGI_REPLY_WIRE_LIMIT - wireBytes;
            if (space > remaining) space = remaining;
            if (space == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
            const UmiStatus received = io->read(io->context, frame + end, space, &count);
            if (received != UMI_STATUS_OK) return received;
            if (count == 0U || count > space) return UMI_STATUS_IO_ERROR;
            end += count;
            wireBytes += count;
        } else if (status != UMI_STATUS_OK) {
            return status;
        } else {
            if (record.type == UMI_WEB_FASTCGI_STDOUT && record.bodyBytes != 0U)
                memcpy(body + before, record.body, record.bodyBytes);
            begin += record.consumed;
        }
    }
    return begin == end ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiWebFastCgiExchange(const UmiWebConnectionIo *io, const UmiWebFastCgiRequest *request,
    const UmiCancellationToken *cancel, UmiWebFastCgiResult *result)
{
    if (io == NULL || io->read == NULL || io->write == NULL || request == NULL ||
        result == NULL || result->body != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t capacity = 0U, written = 0U;
    UmiStatus status = RequestCapacity(request, &capacity);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;

    UmiWebFastCgiReply reply;
    status = UmiWebFastCgiReplyInit(request->requestId, request->replyLimit, &reply);
    if (status != UMI_STATUS_OK) return status;
    uint8_t *frame = malloc(FASTCGI_RECEIVE_BYTES);
    uint8_t *wire = malloc(capacity);
    uint8_t *body = malloc(request->replyLimit);
    if (frame == NULL || wire == NULL || body == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = EncodeRequest(request, wire, capacity, frame, &written);
    if (status == UMI_STATUS_OK) status = SendRequest(io, wire, written, cancel);
    free(wire);
    if (status == UMI_STATUS_OK) status = ReceiveReply(io, &reply, frame, body, cancel);
    free(frame);
    if (status == UMI_STATUS_OK) {
        const int successful = reply.applicationStatus == 0U &&
            reply.protocolStatus == UMI_WEB_FASTCGI_REQUEST_COMPLETE;
        *result = (UmiWebFastCgiResult){.reply = reply};
        if (successful && reply.stdoutBytes != 0U) {
            result->body = body;
            result->bodyBytes = reply.stdoutBytes;
            body = NULL;
        }
    }
    free(body);
    return status;
}

void UmiWebFastCgiResultDestroy(UmiWebFastCgiResult *result)
{
    if (result != NULL) {
        free(result->body);
        *result = (UmiWebFastCgiResult){0};
    }
}
