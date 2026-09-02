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
/*
 * Initialise payments approval policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_approval_policy_init(UmiPaymentsApprovalPolicy *value,
    const char *id,
    uint32_t makers_required,
    uint32_t makers_received,
    uint32_t checkers_required,
    uint32_t checkers_received,
    bool segregated) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->makers_required=makers_required;
    value->makers_received=makers_received;
    value->checkers_required=checkers_required;
    value->checkers_received=checkers_received;
    value->segregated=segregated;
    return umi_payments_approval_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments approval policy satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_approval_policy_valid(const UmiPaymentsApprovalPolicy *value) {
    return value!=NULL && (value->makers_required>0U && value->checkers_required>0U && value->makers_received<=value->makers_required && value->checkers_received<=value->checkers_required);
}

/*
 * Provide the payments approval policy satisfied operation used by this module and its
 * client applications.
 */
bool umi_payments_approval_policy_satisfied(const UmiPaymentsApprovalPolicy *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->segregated && value->makers_received==value->makers_required && value->checkers_received==value->checkers_required;
}
