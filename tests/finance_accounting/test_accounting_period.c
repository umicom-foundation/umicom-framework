/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_period.c
 *
 * PURPOSE:
 *   Exercise accounting period validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_period.h"
int main(void) {
    UmiAccountingAccountingPeriod v;
    if(umi_accounting_accounting_period_init(&v, "2026-08", (UmiFinancialDate){2026,8U,1U}, (UmiFinancialDate){2026,8U,31U}, UMI_ACCOUNTING_PERIOD_OPEN)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_accounting_period_open(&v)) return 2;
    return 0;
}
