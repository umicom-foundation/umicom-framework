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

/*
 * Initialise ai developer approval service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_approval_service_init(
    UmiAiDeveloperApprovalService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(service, 0, sizeof(*service));
    service->sequence = 1U;
    return umi_ai_developer_approval_queue_create(&service->queue);
}

/*
 * Provide the ai developer approval service deinit operation used by this module and its
 * client applications.
 */
void umi_ai_developer_approval_service_deinit(
    UmiAiDeveloperApprovalService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;

    umi_ai_developer_approval_queue_destroy(service->queue);
    (void)memset(service, 0, sizeof(*service));
}

/*
 * Provide the ai developer approval request tool operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(request.approval_id) >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(out_approval_id, request.approval_id);
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer approval request patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_approval_request_patch(
    UmiAiDeveloperApprovalService *service,
    const UmiAiCodingPatch *patch,
    char *out_approval_id,
    size_t capacity)
{
    UmiAiDeveloperApprovalRequest request;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(request.approval_id) >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)strcpy(out_approval_id, request.approval_id);
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer approval approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_approval_approve(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || service->queue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_developer_approval_queue_set_state(
        service->queue,
        approval_id,
        UMI_AI_DEVELOPER_APPROVAL_APPROVED);
}

/*
 * Provide the ai developer approval reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_approval_reject(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || service->queue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_developer_approval_queue_set_state(
        service->queue,
        approval_id,
        UMI_AI_DEVELOPER_APPROVAL_REJECTED);
}

/*
 * Provide the ai developer tool approval callback operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
