/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_limit.c
 *
 * PURPOSE:
 *   Exercise risk limit validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit.h"
int main(void) {
    UmiTreasuryRiskLimit v;
    if (umi_treasury_risk_limit_init(&v, "limit", 1000, 800) != UMI_STATUS_OK) return 1;
    if(umi_treasury_risk_limit_buffer_minor(&v)!=200)return 2;
    return 0;
}
