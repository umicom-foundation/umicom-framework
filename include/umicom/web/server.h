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
#ifdef __cplusplus
}
#endif
#endif
