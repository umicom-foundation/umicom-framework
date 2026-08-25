/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_holiday_calendar.c
 *
 * PURPOSE:
 *   Exercise the holiday calendar financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/holiday_calendar.h"

int main(void)
{
    UmiHolidayCalendar c; CHECK(umi_holiday_calendar_init(&c,"GBLO")==UMI_STATUS_OK); CHECK(umi_holiday_calendar_add(&c,(UmiFinancialDate){2026,12U,25U})==UMI_STATUS_OK); CHECK(!umi_holiday_calendar_is_business_day(&c,(UmiFinancialDate){2026,12U,25U}));
    return 0;
}
