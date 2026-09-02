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

/* Provide the ai policy default operation used by this module and its client applications. */
UmiAiPolicy umi_ai_policy_default(void)
{
    UmiAiPolicy policy = {1, 0, 0, 1, 1};
    return policy;
}

/*
 * Provide the ai policy check provider operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_policy_check_provider(const UmiAiPolicy *policy,
                                       UmiAiProviderKind kind,
                                       int approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_AI_PROVIDER_REMOTE) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!policy->allow_remote) return UMI_STATUS_PERMISSION_DENIED;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (policy->require_remote_approval && !approved) return UMI_STATUS_PERMISSION_DENIED;
        return UMI_STATUS_OK;
    }
    return policy->allow_local ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}

/*
 * Provide the ai policy check tool operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_policy_check_tool(const UmiAiPolicy *policy, int approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->allow_tools) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_tool_approval && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
