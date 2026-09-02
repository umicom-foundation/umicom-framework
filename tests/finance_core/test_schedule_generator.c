/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_schedule_generator.c
 *
 * PURPOSE:
 *   Exercise the schedule generator financial-core contract.
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
#include "umicom/finance/core/schedule_generator.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHolidayCalendar c; UmiSchedule s; UmiScheduleRule r={(UmiFinancialDate){2026,1U,1U},(UmiFinancialDate){2027,1U,1U},{3U,UMI_TENOR_MONTHS},UMI_BUSINESS_DAY_FOLLOWING}; CHECK(umi_holiday_calendar_init(&c,"W")==UMI_STATUS_OK); CHECK(umi_schedule_generate(&r,&c,&s)==UMI_STATUS_OK); CHECK(s.count==4U);
    return 0;
}
