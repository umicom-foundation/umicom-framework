/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_heartbeat.c
 *
 * PURPOSE:
 *   Exercise track liveness deadlines for isolated extension-host sessions.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/host_heartbeat.h"
int main(void) { UmiPluginExtensionHostHostHeartbeat h; umi_plugin_extension_host_host_heartbeat_init(&h,100U,300U); umi_plugin_extension_host_host_heartbeat_received(&h,1000U); if(umi_plugin_extension_host_host_heartbeat_expired(&h,1200U)) return 1; if(!umi_plugin_extension_host_host_heartbeat_expired(&h,1401U)) return 2; return 0; }
