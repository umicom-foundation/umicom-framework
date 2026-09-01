/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/policy.c
 *
 * PURPOSE:
 *   Enforce provider and tool policy decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The default is deliberately conservative: local inference is allowed, while remote calls and tools require an explicit policy change.
 */

#include "umicom/ai/policy.h"
#include <stddef.h>

UmiAiPolicy umi_ai_policy_default(void)
{
    UmiAiPolicy policy = {1, 0, 0, 1, 1};
    return policy;
}

UmiStatus umi_ai_policy_check_provider(const UmiAiPolicy *policy,
                                       UmiAiProviderKind kind,
                                       int approved)
{
    if (policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (kind == UMI_AI_PROVIDER_REMOTE) {
        if (!policy->allow_remote) return UMI_STATUS_PERMISSION_DENIED;
        if (policy->require_remote_approval && !approved) return UMI_STATUS_PERMISSION_DENIED;
        return UMI_STATUS_OK;
    }
    return policy->allow_local ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}

UmiStatus umi_ai_policy_check_tool(const UmiAiPolicy *policy, int approved)
{
    if (policy == NULL || !policy->allow_tools) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (policy->require_tool_approval && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
