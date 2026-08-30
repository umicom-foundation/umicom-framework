/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/checksum_policy.c
 *
 * PURPOSE:
 *   Apply checksum evidence requirements before package trust or installation.
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
#include <string.h>
void umi_plugin_extension_host_checksum_policy_init(UmiPluginExtensionHostChecksumPolicy *policy) { if(policy!=NULL) { policy->require_digest=1; policy->require_match=1; policy->minimum_hex_length=64U; } }
UmiPluginExtensionHostDecision umi_plugin_extension_host_checksum_policy_evaluate(const UmiPluginExtensionHostChecksumPolicy *policy,const char *digest,int digest_matches) { size_t n; if(policy==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; if(digest==NULL||digest[0]=='\0') return policy->require_digest?UMI_PLUGIN_EXTENSION_HOST_DENY:UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL; n=strlen(digest); if(n<policy->minimum_hex_length) return UMI_PLUGIN_EXTENSION_HOST_DENY; if(policy->require_match&&!digest_matches) return UMI_PLUGIN_EXTENSION_HOST_DENY; return UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
