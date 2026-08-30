/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/approval_queue.h
 *
 * PURPOSE:
 *   Retain bounded pending and decided approvals for AI coding tool calls,
 *   governed patches and other sensitive developer operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_QUEUE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_APPROVAL_QUEUE_H
#include "umicom/ai_developer_experience/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperApprovalQueue UmiAiDeveloperApprovalQueue;

UmiStatus umi_ai_developer_approval_queue_create(
    UmiAiDeveloperApprovalQueue **out_queue);
void umi_ai_developer_approval_queue_destroy(
    UmiAiDeveloperApprovalQueue *queue);

UmiStatus umi_ai_developer_approval_queue_add(
    UmiAiDeveloperApprovalQueue *queue,
    const UmiAiDeveloperApprovalRequest *request);

UmiStatus umi_ai_developer_approval_queue_find(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalRequest *out_request);

UmiStatus umi_ai_developer_approval_queue_at(
    const UmiAiDeveloperApprovalQueue *queue,
    size_t index,
    UmiAiDeveloperApprovalRequest *out_request);

UmiStatus umi_ai_developer_approval_queue_set_state(
    UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalState state);

size_t umi_ai_developer_approval_queue_count(
    const UmiAiDeveloperApprovalQueue *queue);

size_t umi_ai_developer_approval_queue_pending_count(
    const UmiAiDeveloperApprovalQueue *queue);

uint64_t umi_ai_developer_approval_queue_revision(
    const UmiAiDeveloperApprovalQueue *queue);

void umi_ai_developer_approval_queue_clear(
    UmiAiDeveloperApprovalQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
