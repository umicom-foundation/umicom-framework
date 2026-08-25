/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_business_day_convention.c
 *
 * PURPOSE:
 *   Exercise the business day convention financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/business_day_convention.h"

int main(void)
{
    UmiHolidayCalendar c; UmiFinancialDate o; CHECK(umi_holiday_calendar_init(&c,"W")==UMI_STATUS_OK); CHECK(umi_business_day_adjust((UmiFinancialDate){2026,8U,29U},&c,UMI_BUSINESS_DAY_FOLLOWING,&o)==UMI_STATUS_OK);
    return 0;
}
