/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/listener.h
 *
 * PURPOSE:
 *   Open and close the platform listener used by the native web server.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_LISTENER_H
#define UMICOM_WEB_LISTENER_H
#include "umicom/web/server_config.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebListener { intptr_t native_handle; int open; uint16_t port; } UmiWebListener;
UmiStatus umi_web_listener_open(const UmiWebServerConfig *config,UmiWebListener *out_listener);
void umi_web_listener_close(UmiWebListener *listener);
#ifdef __cplusplus
}
#endif
#endif
