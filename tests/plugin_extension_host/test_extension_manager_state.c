/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_extension_manager_state.c
 *
 * PURPOSE:
 *   Exercise aggregate installation, activation, host and quarantine counts for Extension Centre.
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
#include "umicom/plugin/extension_host/extension_manager_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostExtensionManagerState s; umi_plugin_extension_host_extension_manager_state_init(&s); umi_plugin_extension_host_extension_manager_state_record(&s,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING,0); /* Apply this branch only when its contract condition is satisfied. */ if(s.running!=1U||umi_plugin_extension_host_extension_manager_state_health(&s)!=UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY) return 1; umi_plugin_extension_host_extension_manager_state_record(&s,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED,0); /* Apply this branch only when its contract condition is satisfied. */ if(umi_plugin_extension_host_extension_manager_state_health(&s)!=UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY) return 2; return 0; }
