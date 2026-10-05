/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_obligation.h
 *
 * PURPOSE:
 *   Represent delivery-versus-payment settlement obligations and state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_OBLIGATION_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_OBLIGATION_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement obligation data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementObligation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_minor;
    int64_t security_quantity;
    UmiTreasurySettlementState state;
} UmiTreasurySettlementObligation;
/**
 * Initialise treasury settlement obligation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_settlement_obligation_init(UmiTreasurySettlementObligation *value,
    const char *id,
    int64_t cash_minor,
    int64_t security_quantity,
    UmiTreasurySettlementState state);
/**
 * Check that treasury settlement obligation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_obligation_valid(const UmiTreasurySettlementObligation *value);
/**
 * Provide the treasury settlement obligation complete operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_obligation_complete(const UmiTreasurySettlementObligation *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_settlement_obligation_archive_encode(const UmiTreasurySettlementObligation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_settlement_obligation_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasurySettlementObligation *value);

#ifdef __cplusplus
}
#endif
#endif
