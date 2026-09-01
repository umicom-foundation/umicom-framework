/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/listener.c
 *
 * PURPOSE:
 *   Delegate listener opening and closing to the operating-system adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/listener.h"
#if defined(_WIN32)
UmiStatus umi_web_listener_platform_open(const UmiWebServerConfig *config,UmiWebListener *out_listener);
void umi_web_listener_platform_close(UmiWebListener *listener);
#else
UmiStatus umi_web_listener_platform_open(const UmiWebServerConfig *config,UmiWebListener *out_listener);
void umi_web_listener_platform_close(UmiWebListener *listener);
#endif
UmiStatus umi_web_listener_open(const UmiWebServerConfig *config,UmiWebListener *out_listener){if(umi_web_server_config_validate(config)!=UMI_STATUS_OK||out_listener==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_web_listener_platform_open(config,out_listener);}
void umi_web_listener_close(UmiWebListener *listener){if(listener!=NULL)umi_web_listener_platform_close(listener);}
