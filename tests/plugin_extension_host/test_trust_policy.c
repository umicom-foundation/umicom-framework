/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_trust_policy.c
 *
 * PURPOSE:
 *   Exercise apply minimum-trust, signature and publisher requirements to activation.
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
#include "umicom/plugin/extension_host/trust_policy.h"
int main(void) { UmiPluginExtensionHostTrustPolicy p; umi_plugin_extension_host_trust_policy_init(&p); if(umi_plugin_extension_host_trust_policy_evaluate(&p,UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED,1,1)!=UMI_PLUGIN_EXTENSION_HOST_ALLOW) return 1; if(umi_plugin_extension_host_trust_policy_evaluate(&p,UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN,1,1)!=UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL) return 2; return 0; }
