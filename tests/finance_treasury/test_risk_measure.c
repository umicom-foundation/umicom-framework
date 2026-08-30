/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_measure.c
 *
 * PURPOSE:
 *   Exercise risk measure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_measure.h"
int main(void) {
    UmiTreasuryRiskMeasure v;
    if (umi_treasury_risk_measure_init(&v, "VAR", -250, 9900U, 1U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_risk_measure_absolute_value_minor(&v)!=250)return 2;
    return 0;
}
