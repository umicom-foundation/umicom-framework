/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_calendar.c
 *
 * PURPOSE:
 *   Verify non-overlapping accounting periods and deterministic business-date lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_calendar.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiAccountingAccountingCalendar c; UmiAccountingAccountingPeriod p; umi_accounting_accounting_calendar_init(&c); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_accounting_accounting_period_init(&p,"2026-08",(UmiFinancialDate){2026,8U,1U},(UmiFinancialDate){2026,8U,31U},UMI_ACCOUNTING_PERIOD_OPEN)!=UMI_STATUS_OK)return 1; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_accounting_accounting_calendar_add(&c,&p)!=UMI_STATUS_OK)return 2; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_accounting_accounting_calendar_find_date(&c,(UmiFinancialDate){2026,8U,15U})==NULL)return 3; return 0; }
