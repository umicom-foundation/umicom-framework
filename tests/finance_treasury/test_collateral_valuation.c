/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_valuation.c
 *
 * PURPOSE:
 *   Exercise collateral valuation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_valuation.h"
int main(void) {
    UmiTreasuryCollateralValuation v;
    if (umi_treasury_collateral_valuation_init(&v, "val", 10000, 500U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_collateral_valuation_eligible_value_minor(&v)!=9500)return 2;
    return 0;
}
