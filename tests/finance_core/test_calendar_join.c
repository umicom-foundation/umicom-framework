/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_calendar_join.c
 *
 * PURPOSE:
 *   Exercise the calendar join financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/calendar_join.h"

int main(void)
{
    UmiHolidayCalendar c; UmiCalendarJoin j; CHECK(umi_holiday_calendar_init(&c,"C")==UMI_STATUS_OK); umi_calendar_join_init(&j,UMI_CALENDAR_ALL_OPEN); CHECK(umi_calendar_join_add(&j,&c)==UMI_STATUS_OK);
    return 0;
}
