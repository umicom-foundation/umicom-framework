/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/fastcgi_exchange.h
 *
 * PURPOSE:
 *   Exchange one admitted FastCGI request over an existing bounded transport
 *   and publish complete worker output under explicit caller ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_FASTCGI_EXCHANGE_H
#define UMICOM_WEB_FASTCGI_EXCHANGE_H

#include "umicom/web/connection.h"
#include "umicom/web/fastcgi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_FASTCGI_EXCHANGE_LIMIT (1024U * 1024U)
#define UMI_WEB_FASTCGI_REPLY_WIRE_LIMIT (4U * UMI_WEB_FASTCGI_EXCHANGE_LIMIT)
#define UMI_WEB_FASTCGI_MAX_PARAMETERS 64U

typedef struct UmiWebFastCgiVariable {
    const char *name;
    size_t nameBytes;
    const void *value;
    size_t valueBytes;
} UmiWebFastCgiVariable;

typedef struct UmiWebFastCgiRequest {
    uint16_t requestId;
    const UmiWebFastCgiVariable *variables;
    size_t variableCount;
    const void *body;
    size_t bodyBytes;
    size_t replyLimit;
} UmiWebFastCgiRequest;

typedef struct UmiWebFastCgiResult {
    UmiWebFastCgiReply reply;
    uint8_t *body;
    size_t bodyBytes;
} UmiWebFastCgiResult;

/** Send one complete request to an already connected worker. The caller selects
 * the trusted worker and CGI variables. Duplicate variables and invalid frames
 * are refused before I/O. The full encoded request and combined reply body
 * must each fit UMI_WEB_FASTCGI_EXCHANGE_LIMIT. Record overhead and padding
 * also count toward UMI_WEB_FASTCGI_REPLY_WIRE_LIMIT.
 *
 * The callbacks must bound their waits. Cancel is checked between transfers.
 * Use a worker that consumes its request before waiting for response reads.
 * On any failure, close this transport since some request bytes may have
 * reached the worker. No retry, connection reuse or process launch is performed.
 *
 * Initialize result to zero and destroy it before reuse. Failure leaves it
 * unchanged. OK means a complete worker result. Only a successful protocol
 * and application status publish stdout in result->body. stderr is counted
 * without publication. The caller interprets CGI headers before using the
 * returned bytes in an HTTP response.
 */
UmiStatus UmiWebFastCgiExchange(const UmiWebConnectionIo *io,
    const UmiWebFastCgiRequest *request, const UmiCancellationToken *cancel,
    UmiWebFastCgiResult *result);

void UmiWebFastCgiResultDestroy(UmiWebFastCgiResult *result);

#ifdef __cplusplus
}
#endif
#endif
