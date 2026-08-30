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
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->idempotency_key,sizeof value->idempotency_key,idempotency_key);
    if(rc!=UMI_STATUS_OK)return rc;
    value->fingerprint=fingerprint;
    return umi_payments_idempotency_record_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value) {
    return value!=NULL && (value->idempotency_key[0]!='\0' && value->fingerprint!=0U);
}

bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value) {
    if(value==NULL) return (bool)0;
    return value->fingerprint!=0U;
}
