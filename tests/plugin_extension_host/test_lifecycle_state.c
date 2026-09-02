/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_lifecycle_state.c
 *
 * PURPOSE:
 *   Exercise track the extension lifecycle without allowing invalid state regression.
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
#include "umicom/plugin/extension_host/lifecycle_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostLifecycleState v; umi_plugin_extension_host_lifecycle_state_init(&v); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_lifecycle_state_set(&v,UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING,10U)!=UMI_STATUS_OK||!umi_plugin_extension_host_lifecycle_state_active(&v)) return 1; return 0; }
