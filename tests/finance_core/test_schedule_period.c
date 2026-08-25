/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_schedule_period.c
 *
 * PURPOSE:
 *   Exercise the schedule period financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/schedule_period.h"

int main(void)
{
    UmiHolidayCalendar c; UmiSchedulePeriod p; CHECK(umi_holiday_calendar_init(&c,"W")==UMI_STATUS_OK); CHECK(umi_schedule_period_init(&p,0U,(UmiFinancialDate){2026,1U,1U},(UmiFinancialDate){2026,2U,1U},&c,UMI_BUSINESS_DAY_FOLLOWING)==UMI_STATUS_OK);
    return 0;
}
