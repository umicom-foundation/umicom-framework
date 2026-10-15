/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_bucket.h
 *
 * PURPOSE:
 *   Describe a liquidity time bucket and calculate its contractual gap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_BUCKET_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_BUCKET_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury liquidity bucket data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryLiquidityBucket {
    char id[UMI_TREASURY_ID_CAPACITY];
    int32_t start_days;
    int32_t end_days;
    int64_t inflow_minor;
    int64_t outflow_minor;
} UmiTreasuryLiquidityBucket;
/**
 * Initialise treasury liquidity bucket from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_bucket_init(UmiTreasuryLiquidityBucket *value,
    const char *id,
    int32_t start_days,
    int32_t end_days,
    int64_t inflow_minor,
    int64_t outflow_minor);
/**
 * Check that treasury liquidity bucket satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_liquidity_bucket_valid(const UmiTreasuryLiquidityBucket *value);
/**
 * Provide the treasury liquidity bucket gap minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_liquidity_bucket_gap_minor(const UmiTreasuryLiquidityBucket *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_liquidity_bucket_archive_encode(const UmiTreasuryLiquidityBucket *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_liquidity_bucket_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryLiquidityBucket *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_treasury_liquidity_bucket_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTreasuryLiquidityBucket *expected, UmiTreasuryLiquidityBucket *value);


#ifdef __cplusplus
}
#endif
#endif
