/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/fastcgi_reply.c
 *
 * PURPOSE:
 *   Track one FastCGI reply, preserve stream boundaries and enforce a
 *   shared body budget before the transport accepts a complete result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/fastcgi.h"

UmiStatus UmiWebFastCgiReplyInit(uint16_t requestId, size_t bodyLimit, UmiWebFastCgiReply *reply)
{
    if (reply == NULL || requestId == 0U || bodyLimit == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *reply = (UmiWebFastCgiReply){.requestId = requestId, .bodyLimit = bodyLimit};
    return UMI_STATUS_OK;
}

UmiStatus UmiWebFastCgiReadReply(UmiWebFastCgiReply *reply, const void *wire,
    size_t available, UmiWebFastCgiRecord *record)
{
    if (reply == NULL || record == NULL || reply->requestId == 0U || reply->bodyLimit == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (reply->complete || reply->stdoutBytes > reply->bodyLimit ||
        reply->stderrBytes > reply->bodyLimit - reply->stdoutBytes)
        return UMI_STATUS_INVALID_STATE;

    UmiWebFastCgiRecord frame;
    const UmiStatus status = UmiWebFastCgiParseRecord(wire, available, &frame);
    if (status != UMI_STATUS_OK) return status;
    if (frame.requestId != reply->requestId) return UMI_STATUS_PERMISSION_DENIED;
    UmiWebFastCgiReply next = *reply;

    switch (frame.type) {
    case UMI_WEB_FASTCGI_END_REQUEST:
        if (frame.bodyBytes != 8U || frame.body[4] > UMI_WEB_FASTCGI_UNKNOWN_ROLE)
            return UMI_STATUS_PARSE_ERROR;
        next.applicationStatus = ((uint32_t)frame.body[0] << 24U) | ((uint32_t)frame.body[1] << 16U) |
            ((uint32_t)frame.body[2] << 8U) | frame.body[3];
        next.protocolStatus = (UmiWebFastCgiProtocolStatus)frame.body[4];
        next.stdoutClosed = 1;
        next.stderrClosed = 1;
        next.complete = 1;
        break;
    case UMI_WEB_FASTCGI_STDOUT:
    case UMI_WEB_FASTCGI_STDERR: {
        /* Stream endings are independent. Errors are never added to the
         * application's response, but both streams consume the same budget. */
        const int isOutput = frame.type == UMI_WEB_FASTCGI_STDOUT;
        size_t *total = isOutput ? &next.stdoutBytes : &next.stderrBytes;
        int *closed = isOutput ? &next.stdoutClosed : &next.stderrClosed;
        if (*closed) return UMI_STATUS_PARSE_ERROR;
        if (frame.bodyBytes > next.bodyLimit - next.stdoutBytes - next.stderrBytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        *total += frame.bodyBytes;
        *closed = frame.bodyBytes == 0U;
        break;
    }
    default:
        return UMI_STATUS_PARSE_ERROR;
    }
    *reply = next;
    *record = frame;
    return UMI_STATUS_OK;
}
