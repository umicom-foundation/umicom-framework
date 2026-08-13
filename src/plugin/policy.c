/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/policy.c
 *
 * PURPOSE:
 *   Produce explicit allow, deny and isolation decisions for extension code.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/plugin/policy.h"

#include <stdio.h>
#include <string.h>

UmiPluginPolicy umi_plugin_policy_default(void)
{
    UmiPluginPolicy policy;
    policy.require_signature = 0;
    policy.allow_native_code = 1;
    policy.allow_network = 0;
    policy.allow_process_execution = 0;
    policy.minimum_trust = UMI_PLUGIN_TRUST_LOCAL;
    policy.untrusted_isolation = UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS;
    return policy;
}

static int manifest_requests(const UmiPluginManifest *manifest,
                             const char *permission)
{
    size_t index;
    for (index = 0U; index < manifest->permission_count; ++index) {
        if (strcmp(manifest->permissions[index], permission) == 0) return 1;
    }
    return 0;
}

UmiStatus umi_plugin_policy_evaluate(const UmiPluginPolicy *policy,
                                     const UmiPluginManifest *manifest,
                                     const UmiPluginSignatureDecision *signature,
                                     const UmiPluginPermissionDecision *permissions,
                                     UmiPluginTrustLevel trust,
                                     UmiPluginPolicyDecision *out_decision)
{
    if (policy == NULL || manifest == NULL || permissions == NULL ||
        out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->trust = trust;
    out_decision->requested_permissions = permissions->requested;
    out_decision->denied_permissions = permissions->denied;
    out_decision->isolation = trust >= UMI_PLUGIN_TRUST_VERIFIED
        ? UMI_PLUGIN_ISOLATION_IN_PROCESS : policy->untrusted_isolation;

    if (trust == UMI_PLUGIN_TRUST_BLOCKED) {
        out_decision->isolation = UMI_PLUGIN_ISOLATION_DENIED;
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "publisher or package is blocked");
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (trust < policy->minimum_trust) {
        out_decision->isolation = UMI_PLUGIN_ISOLATION_DENIED;
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "trust level %s is below policy minimum",
                       umi_plugin_trust_level_text(trust));
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (policy->require_signature &&
        (signature == NULL || !signature->verified)) {
        out_decision->isolation = UMI_PLUGIN_ISOLATION_DENIED;
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "a verified package signature is required");
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (permissions->denied > 0U) {
        out_decision->isolation = UMI_PLUGIN_ISOLATION_DENIED;
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "permission not granted: %s", permissions->first_denied);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!policy->allow_network && manifest_requests(manifest, "network.*")) {
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "network access is disabled by product policy");
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!policy->allow_process_execution &&
        manifest_requests(manifest, "process.execute")) {
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "process execution is disabled by product policy");
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!policy->allow_native_code && manifest->library_path[0] != '\0') {
        (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                       "native extension libraries are disabled");
        return UMI_STATUS_PERMISSION_DENIED;
    }
    out_decision->allowed = 1;
    (void)snprintf(out_decision->reason, sizeof(out_decision->reason),
                   "allowed with %s isolation",
                   umi_plugin_isolation_mode_text(out_decision->isolation));
    return UMI_STATUS_OK;
}

const char *umi_plugin_isolation_mode_text(UmiPluginIsolationMode mode)
{
    switch (mode) {
        case UMI_PLUGIN_ISOLATION_IN_PROCESS: return "in-process";
        case UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS: return "restricted-process";
        case UMI_PLUGIN_ISOLATION_SEPARATE_PROCESS: return "separate-process";
        case UMI_PLUGIN_ISOLATION_DENIED: return "denied";
        default: return "unknown";
    }
}

const char *umi_plugin_trust_level_text(UmiPluginTrustLevel level)
{
    switch (level) {
        case UMI_PLUGIN_TRUST_UNKNOWN: return "unknown";
        case UMI_PLUGIN_TRUST_LOCAL: return "local";
        case UMI_PLUGIN_TRUST_VERIFIED: return "verified";
        case UMI_PLUGIN_TRUST_PLATFORM: return "platform";
        case UMI_PLUGIN_TRUST_BLOCKED: return "blocked";
        default: return "invalid";
    }
}
