/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_counterparty_exposure.c
 *
 * PURPOSE:
 *   Exercise counterparty exposure validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/counterparty_exposure.h"
int main(void) {
    UmiTreasuryCounterpartyExposure v;
    if (umi_treasury_counterparty_exposure_init(&v, "cpty", 300, 200) != UMI_STATUS_OK) return 1;
    if(umi_treasury_counterparty_exposure_total_minor(&v)!=500)return 2;
    return 0;
}
