/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_credit_risk_snapshot.c
 *
 * PURPOSE:
 *   Exercise credit risk snapshot validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/credit_risk_snapshot.h"
int main(void) {
    UmiTreasuryCreditRiskSnapshot v;
    if (umi_treasury_credit_risk_snapshot_init(&v, "snap", 100, 50) != UMI_STATUS_OK) return 1;
    if(umi_treasury_credit_risk_snapshot_combined_absolute_minor(&v)!=150)return 2;
    return 0;
}
