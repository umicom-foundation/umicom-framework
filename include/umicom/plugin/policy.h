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

typedef enum UmiPluginIsolationMode {
    UMI_PLUGIN_ISOLATION_IN_PROCESS = 0,
    UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS = 1,
    UMI_PLUGIN_ISOLATION_SEPARATE_PROCESS = 2,
    UMI_PLUGIN_ISOLATION_DENIED = 3
} UmiPluginIsolationMode;

typedef enum UmiPluginTrustLevel {
    UMI_PLUGIN_TRUST_UNKNOWN = 0,
    UMI_PLUGIN_TRUST_LOCAL = 1,
    UMI_PLUGIN_TRUST_VERIFIED = 2,
    UMI_PLUGIN_TRUST_PLATFORM = 3,
    UMI_PLUGIN_TRUST_BLOCKED = 4
} UmiPluginTrustLevel;

typedef struct UmiPluginPolicy {
    int require_signature;
    int allow_native_code;
    int allow_network;
    int allow_process_execution;
    UmiPluginTrustLevel minimum_trust;
    UmiPluginIsolationMode untrusted_isolation;
} UmiPluginPolicy;

typedef struct UmiPluginPolicyDecision {
    int allowed;
    UmiPluginIsolationMode isolation;
    UmiPluginTrustLevel trust;
    size_t requested_permissions;
    size_t denied_permissions;
    char reason[256];
} UmiPluginPolicyDecision;

UmiPluginPolicy umi_plugin_policy_default(void);
UmiStatus umi_plugin_policy_evaluate(const UmiPluginPolicy *policy,
                                     const UmiPluginManifest *manifest,
                                     const UmiPluginSignatureDecision *signature,
                                     const UmiPluginPermissionDecision *permissions,
                                     UmiPluginTrustLevel trust,
                                     UmiPluginPolicyDecision *out_decision);
const char *umi_plugin_isolation_mode_text(UmiPluginIsolationMode mode);
const char *umi_plugin_trust_level_text(UmiPluginTrustLevel level);

#ifdef __cplusplus
}
#endif

#endif
