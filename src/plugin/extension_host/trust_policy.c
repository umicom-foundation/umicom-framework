/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/extension_host/trust_policy.c
 *
 * PURPOSE:
 *   Apply minimum-trust, signature and publisher requirements to activation.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/trust_policy.h"
void umi_plugin_extension_host_trust_policy_init(UmiPluginExtensionHostTrustPolicy *policy) { if(policy!=NULL) { policy->minimum_trust=UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED; policy->require_signature=1; policy->require_known_publisher=1; policy->allow_approval_override=1; } }
UmiPluginExtensionHostDecision umi_plugin_extension_host_trust_policy_evaluate(const UmiPluginExtensionHostTrustPolicy *policy,UmiPluginExtensionHostTrust trust,int signature_valid,int publisher_known) { if(policy==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; if((policy->require_signature&&!signature_valid)||(policy->require_known_publisher&&!publisher_known)) return policy->allow_approval_override?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; if(trust>=policy->minimum_trust) return UMI_PLUGIN_EXTENSION_HOST_ALLOW; return policy->allow_approval_override?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; }
