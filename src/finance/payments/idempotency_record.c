/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/idempotency_record.c
 *
 * PURPOSE:
 *   Implement bind an idempotency key and request fingerprint to one canonical payment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/idempotency_record.h"
#include <string.h>
/*
 * Initialise payments idempotency record from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint) {
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
    rc=umi_financial_core_copy(value->idempotency_key,sizeof value->idempotency_key,idempotency_key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->fingerprint=fingerprint;
    return umi_payments_idempotency_record_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments idempotency record satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value) {
    return value!=NULL && (value->idempotency_key[0]!='\0' && value->fingerprint!=0U);
}

/*
 * Provide the payments idempotency record replay safe operation used by this module and
 * its client applications.
 */
bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->fingerprint!=0U;
}
