/* Umicom Framework | Governed approval workflow | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_APPROVAL_H
#define UMICOM_SECURITY_APPROVAL_H
#include "umicom/security/governance.h"
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
typedef struct UmiSecurityApprovalStore { UmiSecurityApproval items[UMI_SECURITY_GOVERNANCE_MAX_APPROVALS]; size_t count; uint64_t revision; } UmiSecurityApprovalStore;
UmiStatus umi_security_approval_request(UmiSecurityApprovalStore *store,const UmiSecurityApproval *approval);
const UmiSecurityApproval *umi_security_approval_find(const UmiSecurityApprovalStore *store,const char *approval_id,uint64_t now_ns);
UmiStatus umi_security_approval_decide(UmiSecurityApprovalStore *store,const char *approval_id,const char *decided_by,UmiSecurityApprovalState state,const char *reason);
bool umi_security_approval_valid(const UmiSecurityApproval *approval,uint64_t now_ns);
#endif
