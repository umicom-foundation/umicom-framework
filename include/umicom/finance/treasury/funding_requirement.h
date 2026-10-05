/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_requirement.h
 *
 * PURPOSE:
 *   Calculate a funding requirement from forecast outflows and available liquidity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_REQUIREMENT_H
#define UMICOM_FINANCE_TREASURY_FUNDING_REQUIREMENT_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury funding requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryFundingRequirement {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t required_liquidity_minor;
    int64_t available_liquidity_minor;
} UmiTreasuryFundingRequirement;
/**
 * Initialise treasury funding requirement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_requirement_init(UmiTreasuryFundingRequirement *value,
    const char *id,
    int64_t required_liquidity_minor,
    int64_t available_liquidity_minor);
/**
 * Check that treasury funding requirement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_funding_requirement_valid(const UmiTreasuryFundingRequirement *value);
/**
 * Provide the treasury funding requirement shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_funding_requirement_shortfall_minor(const UmiTreasuryFundingRequirement *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_funding_requirement_archive_encode(const UmiTreasuryFundingRequirement *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_funding_requirement_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryFundingRequirement *value);

#ifdef __cplusplus
}
#endif
#endif
