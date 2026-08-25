/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_result.c
 *
 * PURPOSE:
 *   Exercise risk result validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_result.h"
int main(void) {
    UmiTreasuryRiskResult v;
    if (umi_treasury_risk_result_init(&v, "risk-res", 100, 175, 200) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_risk_result_within_limit(&v))return 2;
    return 0;
}
