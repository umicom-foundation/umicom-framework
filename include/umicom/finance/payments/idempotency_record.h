/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/idempotency_record.h
 *
 * PURPOSE:
 *   Bind an idempotency key and request fingerprint to one canonical payment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#define UMICOM_FINANCE_PAYMENTS_IDEMPOTENCY_RECORD_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments idempotency record data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsIdempotencyRecord {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char idempotency_key[UMI_PAYMENTS_ID_CAPACITY];
    uint64_t fingerprint;
} UmiPaymentsIdempotencyRecord;
/**
 * Initialise payments idempotency record from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint);
/**
 * Check that payments idempotency record satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value);
/**
 * Provide the payments idempotency record replay safe operation used by this module and
 * its client applications.
 */
bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value);
#ifdef __cplusplus
}
#endif
#endif
