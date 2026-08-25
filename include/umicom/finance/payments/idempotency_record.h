/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/idempotency_record.h
 *
 * PURPOSE:
 *   Bind an idempotency key and request fingerprint to one canonical payment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#define UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsIdempotencyRecord {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char idempotency_key[UMI_PAYMENTS_ID_CAPACITY];
    uint64_t fingerprint;
} UmiPaymentsIdempotencyRecord;
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint);
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value);
bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value);
#ifdef __cplusplus
}
#endif
#endif
