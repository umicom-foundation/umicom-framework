/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_buffer.h
 *
 * PURPOSE:
 *   Compare available liquidity against a policy buffer requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_BUFFER_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_BUFFER_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury liquidity buffer data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryLiquidityBuffer {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t available_minor;
    int64_t required_minor;
} UmiTreasuryLiquidityBuffer;
/**
 * Initialise treasury liquidity buffer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_buffer_init(UmiTreasuryLiquidityBuffer *value,
    const char *id,
    int64_t available_minor,
    int64_t required_minor);
/**
 * Check that treasury liquidity buffer satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_liquidity_buffer_valid(const UmiTreasuryLiquidityBuffer *value);
/**
 * Provide the treasury liquidity buffer surplus minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_liquidity_buffer_surplus_minor(const UmiTreasuryLiquidityBuffer *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_liquidity_buffer_archive_encode(const UmiTreasuryLiquidityBuffer *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_liquidity_buffer_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryLiquidityBuffer *value);

#ifdef __cplusplus
}
#endif
#endif
