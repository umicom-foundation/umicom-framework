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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostReconnect r; umi_plugin_extension_host_host_reconnect_init(&r); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_reconnect_failed(&r)!=UMI_STATUS_OK||umi_plugin_extension_host_host_reconnect_delay_ms(&r)!=250U) return 1; umi_plugin_extension_host_host_reconnect_failed(&r); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_reconnect_delay_ms(&r)!=500U) return 2; return 0; }
