/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_repair.c
 *
 * PURPOSE:
 *   Implement track payment exception repair progress before resubmission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_repair.h"
#include <string.h>
/*
 * Initialise payments payment repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_repair_init(UmiPaymentsPaymentRepair *value,
    const char *id,
    const char *payment_id,
    uint32_t issue_count,
    uint32_t resolved_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->issue_count=issue_count;
    value->resolved_count=resolved_count;
    return umi_payments_payment_repair_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment repair satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_repair_valid(const UmiPaymentsPaymentRepair *value) {
    return value!=NULL && (value->issue_count>0U && value->resolved_count<=value->issue_count);
}

/*
 * Provide the payments payment repair ready operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_repair_ready(const UmiPaymentsPaymentRepair *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->resolved_count==value->issue_count;
}
