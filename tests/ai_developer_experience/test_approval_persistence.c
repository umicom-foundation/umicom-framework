/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_approval_persistence.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience approval persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/approval_persistence.h"

int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiDeveloperApprovalQueue *source = NULL;
    UmiAiDeveloperApprovalQueue *restored = NULL;
    UmiAiDeveloperApprovalRequest request = {0};
    UmiAiDeveloperApprovalRequest copy;
    size_t restored_count = 0U;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_ai_developer_approval_queue_create(&source) == UMI_STATUS_OK);
    assert(umi_ai_developer_approval_queue_create(&restored) == UMI_STATUS_OK);

    (void)strcpy(request.approval_id, "approval.1");
    (void)strcpy(request.title, "Build\nProject");
    (void)strcpy(request.summary, "Run the approved build.\nRequires process execution.");
    (void)strcpy(request.permission, "developer.execute");
    (void)strcpy(request.subject_id, "developer.build");
    request.kind = UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL;
    request.state = UMI_AI_DEVELOPER_APPROVAL_PENDING;
    request.risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION;
    request.executable = 1;
    request.tool_call.call_id = 7U;
    (void)strcpy(request.tool_call.tool_id, "developer.build");
    (void)strcpy(
        request.tool_call.arguments_json,
        "{\"program\":\"cmake\",\"arguments\":\"--build build\"}");

    assert(umi_ai_developer_approval_queue_add(
        source, &request) == UMI_STATUS_OK);
    assert(umi_ai_developer_approvals_save(
        store, "approvals", source) == UMI_STATUS_OK);
    assert(umi_ai_developer_approvals_restore(
        store, "approvals", restored, &restored_count) == UMI_STATUS_OK);
    assert(restored_count == 1U);
    assert(umi_ai_developer_approval_queue_find(
        restored, "approval.1", &copy) == UMI_STATUS_OK);
    assert(strcmp(copy.title, request.title) == 0);
    assert(strcmp(copy.summary, request.summary) == 0);
    assert(copy.tool_call.approved == 0);

    umi_ai_developer_approval_queue_destroy(restored);
    umi_ai_developer_approval_queue_destroy(source);
    umi_session_store_destroy(store);
    return 0;
}

