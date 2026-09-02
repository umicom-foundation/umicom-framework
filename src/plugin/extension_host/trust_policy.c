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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/trust_policy.h"
/*
 * Initialise plugin extension host trust policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_trust_policy_init(UmiPluginExtensionHostTrustPolicy *policy) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy!=NULL) { policy->minimum_trust=UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED; policy->require_signature=1; policy->require_known_publisher=1; policy->allow_approval_override=1; } }
/*
 * Provide the plugin extension host trust policy evaluate operation used by this module
 * and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_trust_policy_evaluate(const UmiPluginExtensionHostTrustPolicy *policy,UmiPluginExtensionHostTrust trust,int signature_valid,int publisher_known) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(policy==NULL) return UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if((policy->require_signature&&!signature_valid)||(policy->require_known_publisher&&!publisher_known)) return policy->allow_approval_override?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(trust>=policy->minimum_trust) return UMI_PLUGIN_EXTENSION_HOST_ALLOW; return policy->allow_approval_override?UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL:UMI_PLUGIN_EXTENSION_HOST_DENY; }
