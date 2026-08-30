/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_securities_finance_exposure.c
 *
 * PURPOSE:
 *   Exercise securities finance exposure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_finance_exposure.h"
int main(void) {
    UmiTreasurySecuritiesFinanceExposure v;
    if (umi_treasury_securities_finance_exposure_init(&v, "sfe", 1000, 800) != UMI_STATUS_OK) return 1;
    if(umi_treasury_securities_finance_exposure_unsecured_minor(&v)!=200)return 2;
    return 0;
}
