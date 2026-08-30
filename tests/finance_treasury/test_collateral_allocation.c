/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_allocation.c
 *
 * PURPOSE:
 *   Exercise collateral allocation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_allocation.h"
int main(void) {
    UmiTreasuryCollateralAllocation v;
    if (umi_treasury_collateral_allocation_init(&v, "alloc", 1000, 850) != UMI_STATUS_OK) return 1;
    if(umi_treasury_collateral_allocation_shortfall_minor(&v)!=150)return 2;
    return 0;
}
