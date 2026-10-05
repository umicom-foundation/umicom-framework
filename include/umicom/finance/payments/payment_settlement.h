/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_settlement.h
 *
 * PURPOSE:
 *   Represent payment settlement reference, amount and final settlement evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_SETTLEMENT_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_SETTLEMENT_H
#include "umicom/finance/payments/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment settlement data shared with callers of this public
 * contract.
 */
typedef struct UmiPaymentsPaymentSettlement {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char settlement_reference[UMI_PAYMENTS_ID_CAPACITY];
    int64_t amount_minor;
    bool settled;
} UmiPaymentsPaymentSettlement;
/**
 * Initialise payments payment settlement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_settlement_init(UmiPaymentsPaymentSettlement *value,
    const char *id,
    const char *payment_id,
    const char *settlement_reference,
    int64_t amount_minor,
    bool settled);
/**
 * Check that payments payment settlement satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_settlement_valid(const UmiPaymentsPaymentSettlement *value);
/**
 * Provide the payments payment settlement complete operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_settlement_complete(const UmiPaymentsPaymentSettlement *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_payments_payment_settlement_archive_encode(const UmiPaymentsPaymentSettlement *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_payments_payment_settlement_archive_decode(const void *bytes, size_t byte_count,
    UmiPaymentsPaymentSettlement *value);

#ifdef __cplusplus
}
#endif
#endif
