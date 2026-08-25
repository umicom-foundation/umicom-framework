/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_event.c
 *
 * PURPOSE:
 *   Exercise accounting event validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_event.h"
int main(void) {
    UmiAccountingAccountingEvent v;
    if(umi_accounting_accounting_event_init(&v, "evt-1", "CASH_RECEIPT", (UmiFinancialDate){2026,8U,25U}, 1000, "GBP")!=UMI_STATUS_OK) return 1;
    if(umi_accounting_accounting_event_absolute_minor(&v)!=1000) return 2;
    return 0;
}
