/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/server_config.h
 *
 * PURPOSE:
 *   Validate portable web server configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SERVER_CONFIG_H
#define UMICOM_WEB_SERVER_CONFIG_H
#include <stdint.h>
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebServerConfig { char bind_address[64]; uint16_t port; size_t max_request_bytes; int loopback_only; } UmiWebServerConfig;
UmiWebServerConfig umi_web_server_config_default(void);
UmiStatus umi_web_server_config_validate(const UmiWebServerConfig *config);
#ifdef __cplusplus
}
#endif
#endif
