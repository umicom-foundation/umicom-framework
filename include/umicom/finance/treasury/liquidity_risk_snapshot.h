/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture liquidity-risk gap and liquidity buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury liquidity risk snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryLiquidityRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryLiquidityRiskSnapshot;
/**
 * Initialise treasury liquidity risk snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_liquidity_risk_snapshot_init(UmiTreasuryLiquidityRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
/**
 * Check that treasury liquidity risk snapshot satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_liquidity_risk_snapshot_valid(const UmiTreasuryLiquidityRiskSnapshot *value);
/**
 * Provide the treasury liquidity risk snapshot combined absolute minor operation used by
 * this module and its client applications.
 */
int64_t umi_treasury_liquidity_risk_snapshot_combined_absolute_minor(const UmiTreasuryLiquidityRiskSnapshot *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_liquidity_risk_snapshot_archive_encode(const UmiTreasuryLiquidityRiskSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_liquidity_risk_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryLiquidityRiskSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
