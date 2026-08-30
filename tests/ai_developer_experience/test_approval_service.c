/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_approval_service.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience approval service.
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
#include "umicom/ai_developer_experience/approval_service.h"

int main(void)
{
    UmiAiDeveloperApprovalService service;
    UmiAiCodingToolDescriptor descriptor = {0};
    UmiAiCodingToolCall call = {0};
    UmiAiDeveloperApprovalRequest request;
    char approval_id[128];
    int approved = 1;

    assert(umi_ai_developer_approval_service_init(&service) == UMI_STATUS_OK);

    (void)strcpy(descriptor.tool_id, "developer.build");
    (void)strcpy(descriptor.label, "Build");
    (void)strcpy(descriptor.description, "Build project");
    (void)strcpy(descriptor.permission, "developer.execute");
    descriptor.risk = UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION;
    descriptor.enabled = 1;
    call.call_id = 7U;
    (void)strcpy(call.tool_id, descriptor.tool_id);
    (void)strcpy(call.arguments_json, "{}");

    assert(umi_ai_developer_tool_approval_callback(
        &service, &descriptor, &call, &approved) == UMI_STATUS_OK);
    assert(approved == 0);
    assert(umi_ai_developer_approval_queue_count(service.queue) == 1U);

    assert(umi_ai_developer_approval_queue_at(
        service.queue, 0U, &request) == UMI_STATUS_OK);
    (void)strcpy(approval_id, request.approval_id);
    assert(request.tool_call.approved == 0);

    assert(umi_ai_developer_approval_approve(
        &service, approval_id) == UMI_STATUS_OK);

    umi_ai_developer_approval_service_deinit(&service);
    return 0;
}

