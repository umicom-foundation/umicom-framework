/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/server.h
 *
 * PURPOSE:
 *   Coordinate listener lifecycle and observable native-server state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SERVER_H
#define UMICOM_WEB_SERVER_H
#include "umicom/web/listener.h"
#include "umicom/web/server_state.h"
#include "umicom/web/service.h"
#include "umicom/web/connection.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web server data shared with callers of this public contract.
 */
typedef struct UmiWebServer UmiWebServer;
/**
 * Initialise web server from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_web_server_create(const UmiWebServerConfig *config,UmiWebService *service,UmiWebServer **out_server);
/**
 * Release or reset state held by web server so the same storage can be reused safely.
 */
void umi_web_server_destroy(UmiWebServer *server);
/**
 * Provide the web server start operation used by this module and its client applications.
 */
UmiStatus umi_web_server_start(UmiWebServer *server);
/**
 * Provide the web server stop operation used by this module and its client applications.
 */
UmiStatus umi_web_server_stop(UmiWebServer *server);
/**
 * Provide the web server state operation used by this module and its client applications.
 */
const UmiWebServerState *umi_web_server_state(const UmiWebServer *server);
/* Serve one connection on a started, explicitly loopback-only IPv4 server
 * bound to 127.0.0.1. Native Windows/POSIX adapter; other hosts may return
 * NOT_IMPLEMENTED. wait_ms and exchange_ms are 1..60000. The exchange deadline
 * covers reading and writing, but cannot preempt an application handler.
 * Call from a dedicated owner/worker thread, never a GUI event handler.
 * Keep server/service alive and unchanged until return. Handlers must not
 * stop, start or destroy them; a nested ServeNext returns BUSY. Only Cancel's
 * token may be requested from another thread. The token remains host-owned.
 * Every accepted socket is closed. A timeout/error leaves the listener alive;
 * no request is retried. This is unauthenticated local development transport,
 * not a public hosting, TLS, sandbox or application authorization service. */
UmiStatus UmiWebServerServeNext(UmiWebServer *server,uint32_t wait_ms,uint32_t exchange_ms,
    const UmiCancellationToken *cancel,UmiWebExchangeResult *out);

#ifdef __cplusplus
}
#endif
#endif
