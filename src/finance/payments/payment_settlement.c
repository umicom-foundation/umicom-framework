/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_settlement.c
 *
 * PURPOSE:
 *   Implement represent payment settlement reference, amount and final settlement evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_settlement.h"
#include <string.h>
/*
 * Initialise payments payment settlement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_settlement_init(UmiPaymentsPaymentSettlement *value,
    const char *id,
    const char *payment_id,
    const char *settlement_reference,
    int64_t amount_minor,
    bool settled) {
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
    rc=umi_financial_core_copy(value->settlement_reference,sizeof value->settlement_reference,settlement_reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    value->settled=settled;
    return umi_payments_payment_settlement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment settlement satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_settlement_valid(const UmiPaymentsPaymentSettlement *value) {
    return value!=NULL && (value->settlement_reference[0]!='\0' && value->amount_minor>0);
}

/*
 * Provide the payments payment settlement complete operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_settlement_complete(const UmiPaymentsPaymentSettlement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->settled;
}
