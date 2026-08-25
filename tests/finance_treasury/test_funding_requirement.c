/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_requirement.c
 *
 * PURPOSE:
 *   Exercise funding requirement validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_requirement.h"
int main(void) {
    UmiTreasuryFundingRequirement v;
    if (umi_treasury_funding_requirement_init(&v, "req", 1000, 725) != UMI_STATUS_OK) return 1;
    if(umi_treasury_funding_requirement_shortfall_minor(&v)!=275)return 2;
    return 0;
}
