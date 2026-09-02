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
#ifdef __cplusplus
}
#endif
#endif
