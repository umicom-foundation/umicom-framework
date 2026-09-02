/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_business_day_convention.c
 *
 * PURPOSE:
 *   Exercise the business day convention financial-core contract.
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
#include "umicom/finance/core/business_day_convention.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHolidayCalendar c; UmiFinancialDate o; CHECK(umi_holiday_calendar_init(&c,"W")==UMI_STATUS_OK); CHECK(umi_business_day_adjust((UmiFinancialDate){2026,8U,29U},&c,UMI_BUSINESS_DAY_FOLLOWING,&o)==UMI_STATUS_OK);
    return 0;
}
