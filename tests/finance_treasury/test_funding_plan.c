/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_funding_plan.c
 *
 * PURPOSE:
 *   Exercise funding plan validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_plan.h"
int main(void) {
    UmiTreasuryFundingPlan v;
    if (umi_treasury_funding_plan_init(&v, "plan", 500, 350) != UMI_STATUS_OK) return 1;
    if(umi_treasury_funding_plan_remaining_minor(&v)!=150)return 2;
    return 0;
}
