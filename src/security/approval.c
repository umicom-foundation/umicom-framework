/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/approval.c
 *
 * PURPOSE:
 *   Implement the approval behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Governed approval workflow | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/approval.h"
#include <stdio.h>
#include <string.h>
/* Provide the find mutable operation used by this module and its client applications. */
static UmiSecurityApproval *find_mutable(UmiSecurityApprovalStore *store,const char *approval_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || approval_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(store->items[index].approval_id,approval_id) == 0) return &store->items[index];
    return NULL;
}
/* Check that security approval satisfies its contract before another service relies on it. */
bool umi_security_approval_valid(const UmiSecurityApproval *approval,uint64_t now_ns) { return approval != NULL && approval->state == UMI_SECURITY_APPROVAL_APPROVED && (approval->expires_at_ns == 0U || now_ns < approval->expires_at_ns); }
/*
 * Find security approval while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityApproval *umi_security_approval_find(const UmiSecurityApprovalStore *store,const char *approval_id,uint64_t now_ns)
{
    const UmiSecurityApproval *approval = find_mutable((UmiSecurityApprovalStore *)(void *)store,approval_id);
    return approval != NULL && (approval->expires_at_ns == 0U || now_ns < approval->expires_at_ns) ? approval : NULL;
}
/*
 * Provide the security approval request operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_approval_request(UmiSecurityApprovalStore *store,const UmiSecurityApproval *approval)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || approval == NULL || approval->approval_id[0] == '\0' || approval->subject_id[0] == '\0' || approval->requested_by[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_mutable(store,approval->approval_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_APPROVALS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count] = *approval;
    store->items[store->count].state = UMI_SECURITY_APPROVAL_PENDING;
    store->count += 1U; store->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security approval decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_approval_decide(UmiSecurityApprovalStore *store,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason)
{
    UmiSecurityApproval *approval = find_mutable(store,approval_id);
    int first;
    int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (approval == NULL || decided_by == NULL || reason == NULL || (state != UMI_SECURITY_APPROVAL_APPROVED && state != UMI_SECURITY_APPROVAL_DENIED)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (approval->state != UMI_SECURITY_APPROVAL_PENDING) return UMI_STATUS_INVALID_STATE;
    first = snprintf(approval->decided_by,sizeof(approval->decided_by),"%s",decided_by);
    second = snprintf(approval->reason,sizeof(approval->reason),"%s",reason);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first < 0 || second < 0 || (size_t)first >= sizeof(approval->decided_by) || (size_t)second >= sizeof(approval->reason)) return UMI_STATUS_CAPACITY_EXCEEDED;
    approval->state = state; store->revision += 1U;
    return UMI_STATUS_OK;
}
