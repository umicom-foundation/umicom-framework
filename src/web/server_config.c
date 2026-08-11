/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_config.c
 *
 * PURPOSE:
 *   Implement default web-server configuration and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/server_config.h"
#include <string.h>
UmiWebServerConfig umi_web_server_config_default(void){UmiWebServerConfig c;(void)memset(&c,0,sizeof(c));(void)umi_web_copy_text(c.bind_address,sizeof(c.bind_address),"127.0.0.1");c.port=8080U;c.max_request_bytes=UMI_WEB_BODY_CAPACITY;c.loopback_only=1;return c;}
UmiStatus umi_web_server_config_validate(const UmiWebServerConfig *config){if(config==NULL||config->bind_address[0]=='\0'||config->port==0U)return UMI_STATUS_INVALID_ARGUMENT;if(config->max_request_bytes==0U||config->max_request_bytes>UMI_WEB_BODY_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
