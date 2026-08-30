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
int main(void) { UmiPluginExtensionHostHostMessage m; if(umi_plugin_extension_host_host_message_build(&m,1U,2U,3U,"s","hello")!=UMI_STATUS_OK) return 1; if(!umi_plugin_extension_host_host_message_valid(&m)) return 2; m.sequence=4U; if(umi_plugin_extension_host_host_message_valid(&m)) return 3; return 0; }
