/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/policy.c
 *
 * PURPOSE:
 *   Implement conservative default tool policy. Read-only repository/language
 *   operations are enabled; process, mutation, network and debug-control calls
 *   require explicit policy and approval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/policy.h"

/*
 * Initialise ai coding tool policy from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_coding_tool_policy_init(UmiAiCodingToolPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;

    policy->allowed_capabilities =
        UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ |
        UMI_AI_CODING_TOOL_CAP_WORKSPACE_SEARCH |
        UMI_AI_CODING_TOOL_CAP_LANGUAGE_READ |
        UMI_AI_CODING_TOOL_CAP_DIAGNOSTIC_READ |
        UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_READ |
        UMI_AI_CODING_TOOL_CAP_DEBUG_READ |
        UMI_AI_CODING_TOOL_CAP_AGENT_READ |
        UMI_AI_CODING_TOOL_CAP_AGENT_CONTROL |
        UMI_AI_CODING_TOOL_CAP_CHECKPOINT;

    policy->auto_approved_capabilities =
        UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ |
        UMI_AI_CODING_TOOL_CAP_WORKSPACE_SEARCH |
        UMI_AI_CODING_TOOL_CAP_LANGUAGE_READ |
        UMI_AI_CODING_TOOL_CAP_DIAGNOSTIC_READ |
        UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_READ |
        UMI_AI_CODING_TOOL_CAP_DEBUG_READ |
        UMI_AI_CODING_TOOL_CAP_AGENT_READ |
        UMI_AI_CODING_TOOL_CAP_CHECKPOINT;

    policy->maximum_plan_steps = 16U;
    policy->maximum_output_bytes = UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES;
    policy->allow_network = 0;
    policy->allow_process_execution = 0;
    policy->allow_source_control_mutation = 0;
    policy->allow_debug_control = 0;
    policy->allow_checkpoint_restore = 0;
}

/*
 * Provide the ai coding tool policy check operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_policy_check(
    const UmiAiCodingToolPolicy *policy,
    const UmiAiCodingToolDescriptor *descriptor,
    int approved,
    int *out_approval_required)
{
    int requires_approval;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || descriptor == NULL ||
        out_approval_required == NULL ||
        descriptor->tool_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_approval_required = 0;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!descriptor->enabled) return UMI_STATUS_UNAVAILABLE;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->required_capabilities &
         ~policy->allowed_capabilities) != 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->required_capabilities &
         UMI_AI_CODING_TOOL_CAP_NETWORK) != 0U &&
        !policy->allow_network) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->required_capabilities &
         UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION) != 0U &&
        !policy->allow_process_execution) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->required_capabilities &
         UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE) != 0U &&
        !policy->allow_source_control_mutation) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((descriptor->required_capabilities &
         UMI_AI_CODING_TOOL_CAP_DEBUG_CONTROL) != 0U &&
        !policy->allow_debug_control) {
        return UMI_STATUS_PERMISSION_DENIED;
    }

    requires_approval =
        descriptor->requires_approval ||
        (descriptor->required_capabilities &
         ~policy->auto_approved_capabilities) != 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (requires_approval && !approved) {
        *out_approval_required = 1;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    return UMI_STATUS_OK;
}
