/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/manifest_policy.c
 *
 * PURPOSE:
 *   Define host policy for extension manifests, signatures and permission declarations.
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
#include "umicom/plugin/extension_host/manifest_policy.h"
void umi_plugin_extension_host_manifest_policy_init(UmiPluginExtensionHostManifestPolicy *policy) { if(policy!=NULL) { policy->require_signature=1; policy->require_checksum=1; policy->allow_unknown_fields=0; policy->maximum_permissions=16U; policy->maximum_risk=60U; } }
UmiPluginExtensionHostDecision umi_plugin_extension_host_manifest_policy_evaluate(const UmiPluginExtensionHostManifestPolicy *policy,int signed_package,int checksum_verified,uint32_t permission_count,uint32_t risk) { if(policy==NULL||risk>100U) return UMI_PLUGIN_EXTENSION_HOST_DENY; if((policy->require_signature&&!signed_package)||(policy->require_checksum&&!checksum_verified)||permission_count>policy->maximum_permissions||risk>policy->maximum_risk) return UMI_PLUGIN_EXTENSION_HOST_DENY; return risk>40U?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_ALLOW; }
