/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_approvals_view.c
 *
 * PURPOSE:
 *   Toolkit-neutral view coverage for AI Developer Experience approvals view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/ai_developer_experience/views/approvals.h"
#include "umicom/ui/command_view.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperApprovalQueue *queue = NULL;
    UmiAiDeveloperApprovalRequest approval = {0};
    UmiUiViewModel *view = NULL;
    UmiUiCommandViewAction action;

    assert(umi_ai_developer_approval_queue_create(&queue) == UMI_STATUS_OK);
    (void)strcpy(approval.approval_id, "approval.1");
    (void)strcpy(approval.title, "Build");
    approval.state = UMI_AI_DEVELOPER_APPROVAL_PENDING;
    approval.kind = UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL;
    approval.risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION;

    assert(umi_ai_developer_approval_queue_add(queue, &approval) ==
           UMI_STATUS_OK);
    assert(umi_ai_developer_approvals_view_create(
        "test.approvals", queue, "approval.1", 10U, &view) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(view, 0U, &action) == UMI_STATUS_OK);
    assert(action.enabled);

    umi_ui_view_model_destroy(view);
    umi_ai_developer_approval_queue_destroy(queue);
    return 0;
}

