/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_market_risk_snapshot.c
 *
 * PURPOSE:
 *   Exercise market risk snapshot validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/market_risk_snapshot.h"
int main(void) {
    UmiTreasuryMarketRiskSnapshot v;
    if (umi_treasury_market_risk_snapshot_init(&v, "snap", 100, 50) != UMI_STATUS_OK) return 1;
    if(umi_treasury_market_risk_snapshot_combined_absolute_minor(&v)!=150)return 2;
    return 0;
}
