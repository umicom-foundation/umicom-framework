/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/listener.h
 *
 * PURPOSE:
 *   Open and close the platform listener used by the native web server.
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

#ifndef UMICOM_WEB_LISTENER_H
#define UMICOM_WEB_LISTENER_H
#include "umicom/web/server_config.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web listener data shared with callers of this public contract.
 */
typedef struct UmiWebListener { intptr_t native_handle; int open; uint16_t port; } UmiWebListener;
/**
 * Provide the web listener open operation used by this module and its client applications.
 */
UmiStatus umi_web_listener_open(const UmiWebServerConfig *config,UmiWebListener *out_listener);
/**
 * Provide the web listener close operation used by this module and its client
 * applications.
 */
void umi_web_listener_close(UmiWebListener *listener);
#ifdef __cplusplus
}
#endif
#endif
