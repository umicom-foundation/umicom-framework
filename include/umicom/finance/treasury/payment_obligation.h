/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/payment_obligation.h
 *
 * PURPOSE:
 *   Represent a dated treasury payment obligation and outstanding amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_PAYMENT_OBLIGATION_H
#define UMICOM_FINANCE_TREASURY_PAYMENT_OBLIGATION_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury payment obligation data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryPaymentObligation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t due_epoch_millis;
    int64_t amount_minor;
    int64_t paid_minor;
} UmiTreasuryPaymentObligation;
/**
 * Initialise treasury payment obligation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_payment_obligation_init(UmiTreasuryPaymentObligation *value,
    const char *id,
    int64_t due_epoch_millis,
    int64_t amount_minor,
    int64_t paid_minor);
/**
 * Check that treasury payment obligation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_payment_obligation_valid(const UmiTreasuryPaymentObligation *value);
/**
 * Provide the treasury payment obligation outstanding minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_payment_obligation_outstanding_minor(const UmiTreasuryPaymentObligation *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_payment_obligation_archive_encode(const UmiTreasuryPaymentObligation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_payment_obligation_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryPaymentObligation *value);

#ifdef __cplusplus
}
#endif
#endif
