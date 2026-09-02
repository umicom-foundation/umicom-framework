/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/policy.h
 *
 * PURPOSE:
 *   Evaluate extension trust, signatures, permissions and isolation using a
 *   product-neutral policy contract.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_POLICY_H
#define UMICOM_PLUGIN_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/plugin/manifest.h"
#include "umicom/plugin/permission.h"
#include "umicom/plugin/signature.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named plugin isolation mode values accepted by this public contract.
 */
typedef enum UmiPluginIsolationMode {
    UMI_PLUGIN_ISOLATION_IN_PROCESS = 0,
    UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS = 1,
    UMI_PLUGIN_ISOLATION_SEPARATE_PROCESS = 2,
    UMI_PLUGIN_ISOLATION_DENIED = 3
} UmiPluginIsolationMode;

/**
 * List the named plugin trust level values accepted by this public contract.
 */
typedef enum UmiPluginTrustLevel {
    UMI_PLUGIN_TRUST_UNKNOWN = 0,
    UMI_PLUGIN_TRUST_LOCAL = 1,
    UMI_PLUGIN_TRUST_VERIFIED = 2,
    UMI_PLUGIN_TRUST_PLATFORM = 3,
    UMI_PLUGIN_TRUST_BLOCKED = 4
} UmiPluginTrustLevel;

/**
 * Represent the plugin policy data shared with callers of this public contract.
 */
typedef struct UmiPluginPolicy {
    int require_signature;
    int allow_native_code;
    int allow_network;
    int allow_process_execution;
    UmiPluginTrustLevel minimum_trust;
    UmiPluginIsolationMode untrusted_isolation;
} UmiPluginPolicy;

/**
 * Represent the plugin policy decision data shared with callers of this public contract.
 */
typedef struct UmiPluginPolicyDecision {
    int allowed;
    UmiPluginIsolationMode isolation;
    UmiPluginTrustLevel trust;
    size_t requested_permissions;
    size_t denied_permissions;
    char reason[256];
} UmiPluginPolicyDecision;

/**
 * Provide the plugin policy default operation used by this module and its client
 * applications.
 */
UmiPluginPolicy umi_plugin_policy_default(void);
/**
 * Provide the plugin policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_policy_evaluate(const UmiPluginPolicy *policy,
                                     const UmiPluginManifest *manifest,
                                     const UmiPluginSignatureDecision *signature,
                                     const UmiPluginPermissionDecision *permissions,
                                     UmiPluginTrustLevel trust,
                                     UmiPluginPolicyDecision *out_decision);
/**
 * Provide the plugin isolation mode text operation used by this module and its client
 * applications.
 */
const char *umi_plugin_isolation_mode_text(UmiPluginIsolationMode mode);
/**
 * Provide the plugin trust level text operation used by this module and its client
 * applications.
 */
const char *umi_plugin_trust_level_text(UmiPluginTrustLevel level);

#ifdef __cplusplus
}
#endif

#endif
