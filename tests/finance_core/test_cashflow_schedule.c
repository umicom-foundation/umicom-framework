/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_cashflow_schedule.c
 *
 * PURPOSE:
 *   Exercise the cashflow schedule financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/cashflow_schedule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCashflowSchedule s; UmiCashflow c; UmiMoney m={1,2U,{{'G','B','P','\0'}}}; umi_cashflow_schedule_init(&s); CHECK(umi_cashflow_init(&c,"C","T",m,UMI_FINANCIAL_DIRECTION_PAY,(UmiFinancialDate){2026,9U,1U})==UMI_STATUS_OK); CHECK(umi_cashflow_schedule_add(&s,&c)==UMI_STATUS_OK);
    return 0;
}
