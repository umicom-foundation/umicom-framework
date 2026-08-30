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
static UmiSecurityApproval *find_mutable(UmiSecurityApprovalStore *store,const char *approval_id)
{
    size_t index;
    if (store == NULL || approval_id == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].approval_id,approval_id) == 0) return &store->items[index];
    return NULL;
}
bool umi_security_approval_valid(const UmiSecurityApproval *approval,uint64_t now_ns) { return approval != NULL && approval->state == UMI_SECURITY_APPROVAL_APPROVED && (approval->expires_at_ns == 0U || now_ns < approval->expires_at_ns); }
const UmiSecurityApproval *umi_security_approval_find(const UmiSecurityApprovalStore *store,const char *approval_id,uint64_t now_ns)
{
    const UmiSecurityApproval *approval = find_mutable((UmiSecurityApprovalStore *)(void *)store,approval_id);
    return approval != NULL && (approval->expires_at_ns == 0U || now_ns < approval->expires_at_ns) ? approval : NULL;
}
UmiStatus umi_security_approval_request(UmiSecurityApprovalStore *store,const UmiSecurityApproval *approval)
{
    if (store == NULL || approval == NULL || approval->approval_id[0] == '\0' || approval->subject_id[0] == '\0' || approval->requested_by[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (find_mutable(store,approval->approval_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_APPROVALS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count] = *approval;
    store->items[store->count].state = UMI_SECURITY_APPROVAL_PENDING;
    store->count += 1U; store->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_security_approval_decide(UmiSecurityApprovalStore *store,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason)
{
    UmiSecurityApproval *approval = find_mutable(store,approval_id);
    int first;
    int second;
    if (approval == NULL || decided_by == NULL || reason == NULL || (state != UMI_SECURITY_APPROVAL_APPROVED && state != UMI_SECURITY_APPROVAL_DENIED)) return UMI_STATUS_INVALID_ARGUMENT;
    if (approval->state != UMI_SECURITY_APPROVAL_PENDING) return UMI_STATUS_INVALID_STATE;
    first = snprintf(approval->decided_by,sizeof(approval->decided_by),"%s",decided_by);
    second = snprintf(approval->reason,sizeof(approval->reason),"%s",reason);
    if (first < 0 || second < 0 || (size_t)first >= sizeof(approval->decided_by) || (size_t)second >= sizeof(approval->reason)) return UMI_STATUS_CAPACITY_EXCEEDED;
    approval->state = state; store->revision += 1U;
    return UMI_STATUS_OK;
}
