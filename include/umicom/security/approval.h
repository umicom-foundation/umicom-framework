/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/approval.h
 *
 * PURPOSE:
 *   Publish the public approval contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_APPROVAL_H
#define UMICOM_SECURITY_APPROVAL_H
#include "umicom/security/governance.h"
/**
 * Represent the security approval data shared with callers of this public contract.
 */
typedef struct UmiSecurityApproval {
    char approval_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    UmiSecurityApprovalSubject subject_kind;
    char subject_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char requested_by[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char decided_by[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    UmiSecurityApprovalState state;
    uint64_t requested_at_ns;
    uint64_t expires_at_ns;
    char reason[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
} UmiSecurityApproval;
/**
 * Represent the security approval store data shared with callers of this public contract.
 */
typedef struct UmiSecurityApprovalStore { UmiSecurityApproval items[UMI_SECURITY_GOVERNANCE_MAX_APPROVALS]; size_t count; uint64_t revision; } UmiSecurityApprovalStore;
/**
 * Provide the security approval request operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_approval_request(UmiSecurityApprovalStore *store,const UmiSecurityApproval *approval);
/**
 * Find security approval while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityApproval *umi_security_approval_find(const UmiSecurityApprovalStore *store,const char *approval_id,uint64_t now_ns);
/**
 * Provide the security approval decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_approval_decide(UmiSecurityApprovalStore *store,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason);
/**
 * Check that security approval satisfies its contract before another service relies on it.
 */
bool umi_security_approval_valid(const UmiSecurityApproval *approval,uint64_t now_ns);
#endif
