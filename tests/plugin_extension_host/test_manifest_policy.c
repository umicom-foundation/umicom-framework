/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_manifest_policy.c
 *
 * PURPOSE:
 *   Exercise define host policy for extension manifests, signatures and permission declarations.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/manifest_policy.h"
int main(void) { UmiPluginExtensionHostManifestPolicy p; umi_plugin_extension_host_manifest_policy_init(&p); if(umi_plugin_extension_host_manifest_policy_evaluate(&p,1,1,3U,20U)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; if(umi_plugin_extension_host_manifest_policy_evaluate(&p,0,1,3U,20U)!=UMI_PLUGIN_EXTENSION_HOST_DENY) return 2; return 0; }
