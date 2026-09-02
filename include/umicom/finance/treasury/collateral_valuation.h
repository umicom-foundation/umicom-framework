/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_valuation.h
 *
 * PURPOSE:
 *   Calculate post-haircut collateral value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_VALUATION_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_VALUATION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury collateral valuation data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCollateralValuation {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t gross_value_minor;
    uint32_t haircut_bps;
} UmiTreasuryCollateralValuation;
/**
 * Initialise treasury collateral valuation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_valuation_init(UmiTreasuryCollateralValuation *value,
    const char *id,
    int64_t gross_value_minor,
    uint32_t haircut_bps);
/**
 * Check that treasury collateral valuation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_valuation_valid(const UmiTreasuryCollateralValuation *value);
/**
 * Provide the treasury collateral valuation eligible value minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_valuation_eligible_value_minor(const UmiTreasuryCollateralValuation *value);
#ifdef __cplusplus
}
#endif
#endif
