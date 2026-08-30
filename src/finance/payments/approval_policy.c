/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/approval_policy.c
 *
 * PURPOSE:
 *   Implement model maker-checker approval evidence and segregation-of-duties readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/approval_policy.h"
#include <string.h>
UmiStatus umi_payments_approval_policy_init(UmiPaymentsApprovalPolicy *value,
    const char *id,
    uint32_t makers_required,
    uint32_t makers_received,
    uint32_t checkers_required,
    uint32_t checkers_received,
    bool segregated) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->makers_required=makers_required;
    value->makers_received=makers_received;
    value->checkers_required=checkers_required;
    value->checkers_received=checkers_received;
    value->segregated=segregated;
    return umi_payments_approval_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_approval_policy_valid(const UmiPaymentsApprovalPolicy *value) {
    return value!=NULL && (value->makers_required>0U && value->checkers_required>0U && value->makers_received<=value->makers_required && value->checkers_received<=value->checkers_required);
}

bool umi_payments_approval_policy_satisfied(const UmiPaymentsApprovalPolicy *value) {
    if(value==NULL) return (bool)0;
    return value->segregated && value->makers_received==value->makers_required && value->checkers_received==value->checkers_required;
}
