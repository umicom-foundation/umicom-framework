/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_process_health.c
 *
 * PURPOSE:
 *   Exercise calculate isolated extension-host process health from heartbeat and crash evidence.
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
#include "umicom/plugin/extension_host/host_process_health.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostProcessHealth v={1000U,950U,0U,0U,1}; /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_host_process_health_evaluate(&v,100U)!=UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY) return 1; v.responsive=0; v.crash_count=3U; /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_host_process_health_evaluate(&v,100U)==UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY) return 2; return 0; }
