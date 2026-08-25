/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/trust_policy.h
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_TRUST_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_TRUST_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostTrustPolicy { UmiPluginExtensionHostTrust minimum_trust; int require_signature; int require_known_publisher; int allow_approval_override; } UmiPluginExtensionHostTrustPolicy;
void umi_plugin_extension_host_trust_policy_init(UmiPluginExtensionHostTrustPolicy *policy);
UmiPluginExtensionHostDecision umi_plugin_extension_host_trust_policy_evaluate(const UmiPluginExtensionHostTrustPolicy *policy, UmiPluginExtensionHostTrust trust, int signature_valid, int publisher_known);

#ifdef __cplusplus
}
#endif

#endif
