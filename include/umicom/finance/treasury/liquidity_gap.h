/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_gap.h
 *
 * PURPOSE:
 *   Represent a currency liquidity mismatch for a defined horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_GAP_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury liquidity gap data shared with callers of this public contract.
 */
typedef struct UmiTreasuryLiquidityGap {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t horizon_days;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryLiquidityGap;
/**
 * Initialise treasury liquidity gap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_gap_init(UmiTreasuryLiquidityGap *value,
    const char *id,
    int32_t horizon_days,
    int64_t inflow_minor,
    int64_t outflow_minor);
/**
 * Check that treasury liquidity gap satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_liquidity_gap_valid(const UmiTreasuryLiquidityGap *value);
/**
 * Provide the treasury liquidity gap net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_liquidity_gap_net_minor(const UmiTreasuryLiquidityGap *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_liquidity_gap_archive_encode(const UmiTreasuryLiquidityGap *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_liquidity_gap_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryLiquidityGap *value);

#ifdef __cplusplus
}
#endif
#endif
