/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/overview.c
 *
 * PURPOSE:
 *   Build the AI Developer overview from existing runtime/tool/task/approval
 *   snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/overview.h"

#include "umicom/ai_developer_experience/action_ids.h"

/*
 * Initialise ai developer overview view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_overview_view_create(
    const char *view_id,
    const UmiAiCodingRuntimePlatformSnapshot *coding,
    const UmiAiCodingToolsPlatformSnapshot *tools,
    const UmiAiDeveloperTaskRegistry *tasks,
    const UmiAiDeveloperApprovalQueue *approvals,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coding == NULL || tools == NULL ||
        tasks == NULL || approvals == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-overview",
        "AI Developer",
        "Repository-aware coding, governed changes, approvals, tools and validation.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view,
            "ai-developer.agent-state",
            umi_ai_coding_runtime_state_text(coding->agent.state));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.task-count",
            (int64_t)umi_ai_developer_task_registry_count(tasks));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.pending-approvals",
            (int64_t)umi_ai_developer_approval_queue_pending_count(approvals));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.tool-count",
            (int64_t)tools->tool_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.tool-history",
            (int64_t)tools->result_history_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.chat-sessions",
            (int64_t)tools->chat_session_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-developer.patch-recorded",
            coding->agent.patch_recorded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-developer.validation-passed",
            coding->agent.validation_passed);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_new_chat(),
            "New Chat",
            "Start a repository-aware coding conversation",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_tasks(),
            "Tasks",
            "Inspect coding-agent tasks",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_approvals(),
            "Approvals",
            "Review sensitive developer operations",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 3U,
            umi_ai_developer_action_open_review(),
            "Patch Review",
            "Review the current governed coding patch",
            coding->agent.patch_recorded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 4U,
            umi_ai_developer_action_open_tools(),
            "Tool Activity",
            "Inspect controlled developer-tool execution",
            1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 5U,
            umi_ai_developer_action_open_context(),
            "Context",
            "Inspect repository context available to AI",
            1);

    return status;
}
