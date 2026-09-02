/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_allocation.h
 *
 * PURPOSE:
 *   Represent collateral allocation against a secured requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_ALLOCATION_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_ALLOCATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury collateral allocation data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCollateralAllocation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t required_minor;
    int64_t allocated_minor;
} UmiTreasuryCollateralAllocation;
/**
 * Initialise treasury collateral allocation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_collateral_allocation_init(UmiTreasuryCollateralAllocation *value,
    const char *id,
    int64_t required_minor,
    int64_t allocated_minor);
/**
 * Check that treasury collateral allocation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_allocation_valid(const UmiTreasuryCollateralAllocation *value);
/**
 * Provide the treasury collateral allocation shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_collateral_allocation_shortfall_minor(const UmiTreasuryCollateralAllocation *value);
#ifdef __cplusplus
}
#endif
#endif
