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
#ifdef __cplusplus
}
#endif
#endif
