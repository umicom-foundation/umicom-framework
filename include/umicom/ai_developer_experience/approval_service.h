/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/approval_service.h
 *
 * PURPOSE:
 *   Convert coding-tool approval callbacks and governed patch review into one
 *   reusable Framework approval centre.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_SERVICE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_SERVICE_H

#include "umicom/ai_developer_experience/approval_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai developer approval service data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperApprovalService {
    UmiAiDeveloperApprovalQueue *queue;
    uint64_t sequence;
} UmiAiDeveloperApprovalService;

/**
 * Initialise ai developer approval service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_approval_service_init(
    UmiAiDeveloperApprovalService *service);

/**
 * Provide the ai developer approval service deinit operation used by this module and its
 * client applications.
 */
void umi_ai_developer_approval_service_deinit(
    UmiAiDeveloperApprovalService *service);

/**
 * Provide the ai developer approval request tool operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_approval_request_tool(
    UmiAiDeveloperApprovalService *service,
    const UmiAiCodingToolDescriptor *descriptor,
    const UmiAiCodingToolCall *call,
    char *out_approval_id,
    size_t capacity);

/**
 * Provide the ai developer approval request patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_approval_request_patch(
    UmiAiDeveloperApprovalService *service,
    const UmiAiCodingPatch *patch,
    char *out_approval_id,
    size_t capacity);

/**
 * Provide the ai developer approval approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_approval_approve(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id);

/**
 * Provide the ai developer approval reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_approval_reject(
    UmiAiDeveloperApprovalService *service,
    const char *approval_id);

/**
 * Provide the ai developer tool approval callback operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_tool_approval_callback(
    void *user_data,
    const UmiAiCodingToolDescriptor *descriptor,
    const UmiAiCodingToolCall *call,
    int *out_approved);

#ifdef __cplusplus
}
#endif
#endif
