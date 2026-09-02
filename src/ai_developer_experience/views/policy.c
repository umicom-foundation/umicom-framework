/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/policy.c
 *
 * PURPOSE:
 *   Project controlled AI developer-tool permissions and risk gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/policy.h"

#include "umicom/ai_developer_experience/action_ids.h"

/*
 * Initialise ai developer policy view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_policy_view_create(
    const char *view_id,
    const UmiAiCodingToolEnvironment *environment,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-policy",
        "AI Tool Policy",
        "Capability, mutation, network and approval boundaries for model-requested developer operations.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_view_set_integer(
        *out_view,
        "ai-policy.allowed-capabilities",
        (int64_t)environment->policy.allowed_capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-policy.auto-approved-capabilities",
            (int64_t)environment->policy.auto_approved_capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-policy.allow-network",
            environment->policy.allow_network);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-policy.allow-process-execution",
            environment->policy.allow_process_execution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-policy.allow-source-control-mutation",
            environment->policy.allow_source_control_mutation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-policy.allow-debug-control",
            environment->policy.allow_debug_control);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-policy.allow-checkpoint-restore",
            environment->policy.allow_checkpoint_restore);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-policy.max-plan-steps",
            (int64_t)environment->policy.maximum_plan_steps);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_tools(),
            "Tool Activity",
            "Inspect recent controlled tool execution",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);

    return status;
}
