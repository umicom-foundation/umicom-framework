/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_source.h
 *
 * PURPOSE:
 *   Model a funding facility with capacity, drawn amount and cost.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_SOURCE_H
#define UMICOM_FINANCE_TREASURY_FUNDING_SOURCE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury funding source data shared with callers of this public contract.
 */
typedef struct UmiTreasuryFundingSource {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t capacity_minor;
    int64_t drawn_minor;
    int32_t spread_bps;
} UmiTreasuryFundingSource;
/**
 * Initialise treasury funding source from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_source_init(UmiTreasuryFundingSource *value,
    const char *id,
    int64_t capacity_minor,
    int64_t drawn_minor,
    int32_t spread_bps);
/**
 * Check that treasury funding source satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_funding_source_valid(const UmiTreasuryFundingSource *value);
/**
 * Provide the treasury funding source available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_funding_source_available_minor(const UmiTreasuryFundingSource *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_funding_source_archive_encode(const UmiTreasuryFundingSource *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_funding_source_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryFundingSource *value);

#ifdef __cplusplus
}
#endif
#endif
