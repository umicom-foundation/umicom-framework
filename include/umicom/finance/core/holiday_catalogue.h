/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/holiday_catalogue.h
 *
 * PURPOSE:
 *   Provide a bounded catalogue of holiday calendars.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_HOLIDAY_CATALOGUE_H
#define UMICOM_FINANCE_CORE_HOLIDAY_CATALOGUE_H

#include "umicom/finance/core/holiday_calendar.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHolidayCatalogue { UmiHolidayCalendar items[16U]; size_t count; } UmiHolidayCatalogue;
/* Reset catalogue. */ void umi_holiday_catalogue_init(UmiHolidayCatalogue *c);
/* Add unique calendar. */ UmiStatus umi_holiday_catalogue_add(UmiHolidayCatalogue *c,const UmiHolidayCalendar *item);
/* Find calendar. */ const UmiHolidayCalendar *umi_holiday_catalogue_find(const UmiHolidayCatalogue *c,const char *id);
#ifdef __cplusplus
}
#endif

#endif
