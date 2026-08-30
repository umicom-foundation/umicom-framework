/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/approval_service.c
 *
 * PURPOSE:
 *   Implement approval requests without allowing a model or tool callback to
 *   become its own authorization authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/approval_service.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_developer_approval_service_init(
    UmiAiDeveloperApprovalService *service)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(service, 0, sizeof(*service));
    service->sequence = 1U;
    return umi_ai_developer_approval_queue_create(&service->queue);
}

void umi_ai_developer_approval_service_deinit(
    UmiAiDeveloperApprovalService *service)
{
    if (service == NULL) return;

    umi_ai_developer_approval_queue_destroy(service->queue);
    (void)memset(service, 0, sizeof(*service));
}

UmiStatus umi_ai_developer_approval_request_tool(
    UmiAiDeveloperApprovalService *service,
    const UmiAiCodingToolDescriptor *descriptor,
    const UmiAiCodingToolCall *call,
    char *out_approval_id,
    size_t capacity)
{
    UmiAiDeveloperApprovalRequest request;
    int written;
    UmiStatus status;

    if (service == NULL || service->queue == NULL ||
        descriptor == NULL || call == NULL ||
        out_approval_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&request, 0, sizeof(request));

    written = snprintf(
        request.approval_id,
        sizeof(request.approval_id),
        "approval.tool.%llu",
        (unsigned long long)service->sequence++);
    if (written < 0 ||
        (size_t)written >= sizeof(request.approval_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    request.kind = descriptor->risk ==
            UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION
        ? UMI_AI_DEVELOPER_APPROVAL_SOURCE_CONTROL
        : descriptor->risk == UMI_AI_CODING_TOOL_RISK_DEBUG_CONTROL
            ? UMI_AI_DEVELOPER_APPROVAL_DEBUG_CONTROL
            : UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL;
    request.state = UMI_AI_DEVELOPER_APPROVAL_PENDING;
    request.risk = descriptor->risk;
    request.tool_call = *call;
    request.tool_call.approved = 0;
    request.requested_sequence = service->sequence;
    request.executable = 1;

    (void)snprintf(
        request.title,
        sizeof(request.title),
        "%s",
        descriptor->label);
    (void)snprintf(
        request.summary,
        sizeof(request.summary),
        "%s",
        descriptor->description);
    (void)snprintf(
        request.permission,
        sizeof(request.permission),
        "%s",
        descriptor->permission);
    (void)snprintf(
        request.subject_id,
        sizeof(request.subject_id),
        "%s",
        descriptor->tool_id);

    status = umi_ai_developer_approval_queue_add(
        service->queue, &request);
    if (status != UMI_STATUS_OK) return status;

    if (strlen(request.approval_id) >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(out_approval_id, request.approval_id);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approval_request_patch(
    UmiAiDeveloperApprovalService *service,
    const UmiAiCodingPatch *patch,
    char *out_approval_id,
    size_t capacity)
{
    UmiAiDeveloperApprovalRequest request;
    int written;
    UmiStatus status;

    if (service == NULL || service->queue == NULL ||
        patch == NULL || out_approval_id == NULL ||
        capacity == 0U || patch->patch_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&request, 0, sizeof(request));

    written = snprintf(
        request.approval_id,
        sizeof(request.approval_id),
        "approval.patch.%llu",
        (unsigned long long)service->sequence++);
    if (written < 0 ||
        (size_t)written >= sizeof(request.approval_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    request.kind = UMI_AI_DEVELOPER_APPROVAL_PATCH;
    request.state = UMI_AI_DEVELOPER_APPROVAL_PENDING;
    request.risk = UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION;
    request.requested_sequence = service->sequence;
    request.executable = 1;

    (void)snprintf(
        request.title,
        sizeof(request.title),
        "%.250s",
        patch->title[0] != '\0' ? patch->title : "AI Coding Patch");
    (void)snprintf(
        request.summary,
        sizeof(request.summary),
        "%s",
        patch->rationale);
    (void)snprintf(
        request.permission,
        sizeof(request.permission),
        "%s",
        "agent.patch.approve");
    (void)snprintf(
        request.subject_id,
        sizeof(request.subject_id),
        "%s",
        patch->patch_id);

    status = umi_ai_developer_approval_queue_add(
        service->queue, &request);
    if (status != UMI_STATUS_OK) return status;

    if (strlen(request.approval_id) >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(out_approval_id, request.approval_id);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_approval_approve(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id)
{
    if (service == NULL || service->queue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_developer_approval_queue_set_state(
        service->queue,
        approval_id,
        UMI_AI_DEVELOPER_APPROVAL_APPROVED);
}

UmiStatus umi_ai_developer_approval_reject(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id)
{
    if (service == NULL || service->queue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_developer_approval_queue_set_state(
        service->queue,
        approval_id,
        UMI_AI_DEVELOPER_APPROVAL_REJECTED);
}

UmiStatus umi_ai_developer_tool_approval_callback(
    void *user_data,
    const UmiAiCodingToolDescriptor *descriptor,
    const UmiAiCodingToolCall *call,
    int *out_approved)
{
    UmiAiDeveloperApprovalService *service =
        (UmiAiDeveloperApprovalService *)user_data;
    char approval_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    UmiStatus status;

    if (out_approved == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * This callback records a pending approval but never approves inline.
     * The presentation/controller layer must explicitly decide the request.
     */
    *out_approved = 0;

    status = umi_ai_developer_approval_request_tool(
        service,
        descriptor,
        call,
        approval_id,
        sizeof(approval_id));

    return status;
}
