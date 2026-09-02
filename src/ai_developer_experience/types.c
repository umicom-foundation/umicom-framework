/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for AI developer-experience records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/types.h"

/*
 * Provide the ai developer pane text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_pane_text(UmiAiDeveloperPaneKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_AI_DEVELOPER_PANE_OVERVIEW: return "overview";
        case UMI_AI_DEVELOPER_PANE_CHAT: return "chat";
        case UMI_AI_DEVELOPER_PANE_TASKS: return "tasks";
        case UMI_AI_DEVELOPER_PANE_APPROVALS: return "approvals";
        case UMI_AI_DEVELOPER_PANE_PATCH_REVIEW: return "patch-review";
        case UMI_AI_DEVELOPER_PANE_DIFF: return "diff";
        case UMI_AI_DEVELOPER_PANE_TOOL_ACTIVITY: return "tool-activity";
        case UMI_AI_DEVELOPER_PANE_VALIDATION: return "validation";
        case UMI_AI_DEVELOPER_PANE_CONTEXT: return "context";
        case UMI_AI_DEVELOPER_PANE_HISTORY: return "history";
        case UMI_AI_DEVELOPER_PANE_POLICY: return "policy";
        case UMI_AI_DEVELOPER_PANE_CHECKPOINTS: return "checkpoints";
        default: return "unknown";
    }
}

/*
 * Provide the ai developer approval kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_approval_kind_text(UmiAiDeveloperApprovalKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL: return "tool-call";
        case UMI_AI_DEVELOPER_APPROVAL_PATCH: return "patch";
        case UMI_AI_DEVELOPER_APPROVAL_CHECKPOINT_RESTORE:
            return "checkpoint-restore";
        case UMI_AI_DEVELOPER_APPROVAL_SOURCE_CONTROL:
            return "source-control";
        case UMI_AI_DEVELOPER_APPROVAL_DEBUG_CONTROL:
            return "debug-control";
        default: return "unknown";
    }
}

/*
 * Provide the ai developer approval state text operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_approval_state_text(UmiAiDeveloperApprovalState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_AI_DEVELOPER_APPROVAL_PENDING: return "pending";
        case UMI_AI_DEVELOPER_APPROVAL_APPROVED: return "approved";
        case UMI_AI_DEVELOPER_APPROVAL_REJECTED: return "rejected";
        case UMI_AI_DEVELOPER_APPROVAL_EXECUTED: return "executed";
        case UMI_AI_DEVELOPER_APPROVAL_FAILED: return "failed";
        default: return "unknown";
    }
}

/*
 * Provide the ai developer task state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_task_state_text(UmiAiDeveloperTaskState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_AI_DEVELOPER_TASK_QUEUED: return "queued";
        case UMI_AI_DEVELOPER_TASK_RUNNING: return "running";
        case UMI_AI_DEVELOPER_TASK_REVIEW: return "review";
        case UMI_AI_DEVELOPER_TASK_VALIDATING: return "validating";
        case UMI_AI_DEVELOPER_TASK_COMPLETED: return "completed";
        case UMI_AI_DEVELOPER_TASK_FAILED: return "failed";
        case UMI_AI_DEVELOPER_TASK_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}

/*
 * Provide the ai developer diff line kind text operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_diff_line_kind_text(UmiAiDeveloperDiffLineKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_AI_DEVELOPER_DIFF_CONTEXT: return "context";
        case UMI_AI_DEVELOPER_DIFF_ADD: return "add";
        case UMI_AI_DEVELOPER_DIFF_REMOVE: return "remove";
        default: return "unknown";
    }
}
