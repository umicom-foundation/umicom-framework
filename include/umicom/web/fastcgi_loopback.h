/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/fastcgi_loopback.h
 *
 * PURPOSE:
 *   Give a configured FastCGI route one bounded native loopback connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_FASTCGI_LOOPBACK_H
#define UMICOM_WEB_FASTCGI_LOOPBACK_H

#include "umicom/web/fastcgi_application.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_FASTCGI_LOOPBACK_MAX_WAIT_MS 60000U

typedef struct UmiWebFastCgiLoopbackEndpoint {
    uint16_t port;
    uint32_t timeoutMs;
} UmiWebFastCgiLoopbackEndpoint;

/* Bind a connector to one trusted, explicitly selected port on 127.0.0.1.
 * An open owns a fresh socket and one deadline for connect, writes and reads.
 * The caller keeps endpoint storage immutable while the connector is in use.
 * The cancellation token passed to open must outlive its connection. Close
 * each successful open after the request returns. The caller starts and stops
 * the worker separately and is responsible for selecting the intended process.
 * Invalid or unsupported configuration leaves *out unchanged. */
UmiStatus UmiWebFastCgiLoopbackConnectorInit(const UmiWebFastCgiLoopbackEndpoint *endpoint,
    UmiWebFastCgiConnector *out);

#ifdef __cplusplus
}
#endif

#endif
