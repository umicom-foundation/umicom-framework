/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_calendar.c
 *
 * PURPOSE:
 *   Verify non-overlapping accounting periods and deterministic business-date lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_calendar.h"
int main(void){ UmiAccountingAccountingCalendar c; UmiAccountingAccountingPeriod p; umi_accounting_accounting_calendar_init(&c); if(umi_accounting_accounting_period_init(&p,"2026-08",(UmiFinancialDate){2026,8U,1U},(UmiFinancialDate){2026,8U,31U},UMI_ACCOUNTING_PERIOD_OPEN)!=UMI_STATUS_OK)return 1; if(umi_accounting_accounting_calendar_add(&c,&p)!=UMI_STATUS_OK)return 2; if(umi_accounting_accounting_calendar_find_date(&c,(UmiFinancialDate){2026,8U,15U})==NULL)return 3; return 0; }
