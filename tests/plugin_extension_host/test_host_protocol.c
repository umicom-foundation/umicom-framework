/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_protocol.c
 *
 * PURPOSE:
 *   Exercise maintain protocol sequencing, feature negotiation and replay rejection state.
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
#include "umicom/plugin/extension_host/host_protocol.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPluginExtensionHostHostProtocol p; umi_plugin_extension_host_host_protocol_init(&p,1U,UINT64_C(7)); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_protocol_negotiate(&p,1U,UINT64_C(3))!=UMI_STATUS_OK||p.negotiated_features!=UINT64_C(3)) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_protocol_accept_sequence(&p,1U)!=UMI_STATUS_OK||umi_plugin_extension_host_host_protocol_accept_sequence(&p,1U)==UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_plugin_extension_host_host_protocol_next_sequence(&p)!=1U) return 3; return 0; }
