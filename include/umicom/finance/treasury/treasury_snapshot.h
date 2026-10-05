/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/treasury_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate cash, liquidity, risk and collateral state at a point in time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_TREASURY_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_TREASURY_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury treasury snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryTreasurySnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t cash_minor;
    int64_t liquidity_gap_minor;
    int64_t risk_minor;
    int64_t collateral_minor;
} UmiTreasuryTreasurySnapshot;
/**
 * Initialise treasury treasury snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_snapshot_init(UmiTreasuryTreasurySnapshot *value,
    const char *id,
    int64_t cash_minor,
    int64_t liquidity_gap_minor,
    int64_t risk_minor,
    int64_t collateral_minor);
/**
 * Check that treasury treasury snapshot satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_treasury_snapshot_valid(const UmiTreasuryTreasurySnapshot *value);
/**
 * Provide the treasury treasury snapshot net liquidity minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_treasury_snapshot_net_liquidity_minor(const UmiTreasuryTreasurySnapshot *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_treasury_snapshot_archive_encode(const UmiTreasuryTreasurySnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_treasury_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryTreasurySnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
