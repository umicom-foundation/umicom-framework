/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_holiday_calendar.c
 *
 * PURPOSE:
 *   Exercise the holiday calendar financial-core contract.
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
#include "umicom/finance/core/holiday_calendar.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHolidayCalendar c; CHECK(umi_holiday_calendar_init(&c,"GBLO")==UMI_STATUS_OK); CHECK(umi_holiday_calendar_add(&c,(UmiFinancialDate){2026,12U,25U})==UMI_STATUS_OK); CHECK(!umi_holiday_calendar_is_business_day(&c,(UmiFinancialDate){2026,12U,25U}));
    return 0;
}
