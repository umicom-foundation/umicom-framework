/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_agreement.c
 *
 * PURPOSE:
 *   Exercise collateral agreement validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_agreement.h"
int main(void) {
    UmiTreasuryCollateralAgreement v;
    if (umi_treasury_collateral_agreement_init(&v, "csa", 100, 25) != UMI_STATUS_OK) return 1;
    if(umi_treasury_collateral_agreement_secured_threshold_minor(&v)!=125)return 2;
    return 0;
}
