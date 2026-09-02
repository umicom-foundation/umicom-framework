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

/**
 * Represent the ai developer approval queue data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperApprovalQueue UmiAiDeveloperApprovalQueue;

/**
 * Initialise ai developer approval queue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_approval_queue_create(
    UmiAiDeveloperApprovalQueue **out_queue);
/**
 * Release or reset state held by ai developer approval queue so the same storage can be
 * reused safely.
 */
void umi_ai_developer_approval_queue_destroy(
    UmiAiDeveloperApprovalQueue *queue);

/**
 * Add ai developer approval queue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_developer_approval_queue_add(
    UmiAiDeveloperApprovalQueue *queue,
    const UmiAiDeveloperApprovalRequest *request);

/**
 * Find ai developer approval queue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_developer_approval_queue_find(
    const UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalRequest *out_request);

/**
 * Find ai developer approval queue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ai_developer_approval_queue_at(
    const UmiAiDeveloperApprovalQueue *queue,
    size_t index,
    UmiAiDeveloperApprovalRequest *out_request);

/**
 * Provide the ai developer approval queue set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_approval_queue_set_state(
    UmiAiDeveloperApprovalQueue *queue,
    const char *approval_id,
    UmiAiDeveloperApprovalState state);

/**
 * Return the number of records represented by ai developer approval queue without changing
 * their state.
 */
size_t umi_ai_developer_approval_queue_count(
    const UmiAiDeveloperApprovalQueue *queue);

/**
 * Return the number of records represented by ai developer approval queue pending without
 * changing their state.
 */
size_t umi_ai_developer_approval_queue_pending_count(
    const UmiAiDeveloperApprovalQueue *queue);

/**
 * Provide the ai developer approval queue revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_developer_approval_queue_revision(
    const UmiAiDeveloperApprovalQueue *queue);

/**
 * Release or reset state held by ai developer approval queue so the same storage can be
 * reused safely.
 */
void umi_ai_developer_approval_queue_clear(
    UmiAiDeveloperApprovalQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
