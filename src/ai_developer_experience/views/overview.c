/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/overview.c
 *
 * PURPOSE:
 *   Build the AI Developer overview from existing runtime/tool/task/approval
 *   snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/overview.h"

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_overview_view_create(
    const char *view_id,
    const UmiAiCodingRuntimePlatformSnapshot *coding,
    const UmiAiCodingToolsPlatformSnapshot *tools,
    const UmiAiDeveloperTaskRegistry *tasks,
    const UmiAiDeveloperApprovalQueue *approvals,
    UmiUiViewModel **out_view)
{
    UmiStatus status;

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

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view,
            "ai-developer.agent-state",
            umi_ai_coding_runtime_state_text(coding->agent.state));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.task-count",
            (int64_t)umi_ai_developer_task_registry_count(tasks));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.pending-approvals",
            (int64_t)umi_ai_developer_approval_queue_pending_count(approvals));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.tool-count",
            (int64_t)tools->tool_count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.tool-history",
            (int64_t)tools->result_history_count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-developer.chat-sessions",
            (int64_t)tools->chat_session_count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-developer.patch-recorded",
            coding->agent.patch_recorded);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-developer.validation-passed",
            coding->agent.validation_passed);

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_new_chat(),
            "New Chat",
            "Start a repository-aware coding conversation",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_open_tasks(),
            "Tasks",
            "Inspect coding-agent tasks",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_approvals(),
            "Approvals",
            "Review sensitive developer operations",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 3U,
            umi_ai_developer_action_open_review(),
            "Patch Review",
            "Review the current governed coding patch",
            coding->agent.patch_recorded);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 4U,
            umi_ai_developer_action_open_tools(),
            "Tool Activity",
            "Inspect controlled developer-tool execution",
            1);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 5U,
            umi_ai_developer_action_open_context(),
            "Context",
            "Inspect repository context available to AI",
            1);

    return status;
}
