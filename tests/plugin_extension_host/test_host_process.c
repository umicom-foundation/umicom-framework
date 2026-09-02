/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_process.c
 *
 * PURPOSE:
 *   Exercise track one isolated extension-host process and lifecycle evidence.
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
#include "umicom/plugin/extension_host/host_process.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostProcess p; umi_plugin_extension_host_host_process_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_process_start(&p,"x",10U,100U)!=UMI_STATUS_OK) return 1; umi_plugin_extension_host_host_process_heartbeat(&p,150U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(p.last_heartbeat_ms!=150U) return 2; umi_plugin_extension_host_host_process_crashed(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(p.crash_count!=1U||p.lifecycle!=UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED) return 3; return 0; }
