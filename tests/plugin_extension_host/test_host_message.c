/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_message.c
 *
 * PURPOSE:
 *   Exercise describe one versioned extension-host protocol message.
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
#include "umicom/plugin/extension_host/host_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostMessage m; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_message_build(&m,1U,2U,3U,"s","hello")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_plugin_extension_host_host_message_valid(&m)) return 2; m.sequence=4U; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_message_valid(&m)) return 3; return 0; }
