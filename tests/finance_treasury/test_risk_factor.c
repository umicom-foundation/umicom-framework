/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_factor.c
 *
 * PURPOSE:
 *   Exercise risk factor validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_factor.h"
int main(void) {
    UmiTreasuryRiskFactor v;
    if (umi_treasury_risk_factor_init(&v, "USD-IR", UMI_TREASURY_RISK_MARKET, -25) != UMI_STATUS_OK) return 1;
    if(umi_treasury_risk_factor_absolute_shock_bps(&v)!=25)return 2;
    return 0;
}
