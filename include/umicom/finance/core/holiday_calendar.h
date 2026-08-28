/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/holiday_calendar.h
 *
 * PURPOSE:
 *   Represent explicit holidays and weekends for business-day logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_HOLIDAY_CALENDAR_H
#define UMICOM_FINANCE_CORE_HOLIDAY_CALENDAR_H

#include "umicom/finance/core/business_date.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_HOLIDAY_MAX 128U
typedef struct UmiHolidayCalendar { UmiFinancialId calendar_id; UmiFinancialDate holidays[UMI_HOLIDAY_MAX]; size_t count; uint8_t weekend_mask; } UmiHolidayCalendar;
/* Initialize Saturday/Sunday weekend semantics. */ UmiStatus umi_holiday_calendar_init(UmiHolidayCalendar *c,const char *id);
/* Add a unique holiday. */ UmiStatus umi_holiday_calendar_add(UmiHolidayCalendar *c,UmiFinancialDate d);
/* Test whether a date is a business day. */ bool umi_holiday_calendar_is_business_day(const UmiHolidayCalendar *c,UmiFinancialDate d);
#ifdef __cplusplus
}
#endif

#endif
