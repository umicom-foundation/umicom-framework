/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_host_handshake.c
 *
 * PURPOSE:
 *   Exercise validate host/plugin protocol and ABI handshake evidence before session activation.
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
#include "umicom/plugin/extension_host/host_handshake.h"
int main(void) { uint64_t n=0U; UmiPluginExtensionHostHostHandshake v={1U,1U,2U,1U,3U,UINT64_C(3),UINT64_C(7),1}; if(umi_plugin_extension_host_host_handshake_evaluate(&v,&n)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW||n!=UINT64_C(3)) return 1; v.plugin_protocol=2U; if(umi_plugin_extension_host_host_handshake_evaluate(&v,&n)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 2; return 0; }
