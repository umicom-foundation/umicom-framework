/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_checksum_policy.c
 *
 * PURPOSE:
 *   Exercise apply checksum evidence requirements before package trust or installation.
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
#include "umicom/plugin/extension_host/checksum_policy.h"
int main(void) { UmiPluginExtensionHostChecksumPolicy p; const char *d="0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"; umi_plugin_extension_host_checksum_policy_init(&p); if(umi_plugin_extension_host_checksum_policy_evaluate(&p,d,1)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; if(umi_plugin_extension_host_checksum_policy_evaluate(&p,d,0)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 2; return 0; }
