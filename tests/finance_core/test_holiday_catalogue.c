/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_holiday_catalogue.c
 *
 * PURPOSE:
 *   Exercise the holiday catalogue financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/holiday_catalogue.h"

int main(void)
{
    UmiHolidayCatalogue c; UmiHolidayCalendar h; umi_holiday_catalogue_init(&c); CHECK(umi_holiday_calendar_init(&h,"NYC")==UMI_STATUS_OK); CHECK(umi_holiday_catalogue_add(&c,&h)==UMI_STATUS_OK); CHECK(umi_holiday_catalogue_find(&c,"NYC")!=NULL);
    return 0;
}
