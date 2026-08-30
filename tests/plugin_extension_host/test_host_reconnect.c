/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_reconnect.c
 *
 * PURPOSE:
 *   Exercise calculate bounded reconnect backoff following host interruption.
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
#include "umicom/plugin/extension_host/host_reconnect.h"
int main(void) { UmiPluginExtensionHostHostReconnect r; umi_plugin_extension_host_host_reconnect_init(&r); if(umi_plugin_extension_host_host_reconnect_failed(&r)!=UMI_STATUS_OK||umi_plugin_extension_host_host_reconnect_delay_ms(&r)!=250U) return 1; umi_plugin_extension_host_host_reconnect_failed(&r); if(umi_plugin_extension_host_host_reconnect_delay_ms(&r)!=500U) return 2; return 0; }
