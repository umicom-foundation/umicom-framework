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
typedef struct UmiTreasuryFundingRequirement {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t required_liquidity_minor;
    int64_t available_liquidity_minor;
} UmiTreasuryFundingRequirement;
UmiStatus umi_treasury_funding_requirement_init(UmiTreasuryFundingRequirement *value,
    const char *id,
    int64_t required_liquidity_minor,
    int64_t available_liquidity_minor);
bool umi_treasury_funding_requirement_valid(const UmiTreasuryFundingRequirement *value);
int64_t umi_treasury_funding_requirement_shortfall_minor(const UmiTreasuryFundingRequirement *value);
#ifdef __cplusplus
}
#endif
#endif
