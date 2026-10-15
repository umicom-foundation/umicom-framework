/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_cycle.h
 *
 * PURPOSE:
 *   Define settlement cycle trade-date and settlement-date offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_CYCLE_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_CYCLE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement cycle data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementCycle {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t settlement_days;
} UmiTreasurySettlementCycle;
/**
 * Initialise treasury settlement cycle from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_cycle_init(UmiTreasurySettlementCycle *value,
    const char *id,
    int32_t settlement_days);
/**
 * Check that treasury settlement cycle satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_cycle_valid(const UmiTreasurySettlementCycle *value);
/**
 * Provide the treasury settlement cycle offset days operation used by this module and its
 * client applications.
 */
int32_t umi_treasury_settlement_cycle_offset_days(const UmiTreasurySettlementCycle *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_settlement_cycle_archive_encode(const UmiTreasurySettlementCycle *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_settlement_cycle_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasurySettlementCycle *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_settlement_cycle_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasurySettlementCycle *expected, UmiTreasurySettlementCycle *value);


#ifdef __cplusplus
}
#endif
#endif
