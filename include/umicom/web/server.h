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
typedef struct UmiWebServer UmiWebServer;
UmiStatus umi_web_server_create(const UmiWebServerConfig *config,UmiWebService *service,UmiWebServer **out_server);
void umi_web_server_destroy(UmiWebServer *server);
UmiStatus umi_web_server_start(UmiWebServer *server);
UmiStatus umi_web_server_stop(UmiWebServer *server);
const UmiWebServerState *umi_web_server_state(const UmiWebServer *server);
#ifdef __cplusplus
}
#endif
#endif
