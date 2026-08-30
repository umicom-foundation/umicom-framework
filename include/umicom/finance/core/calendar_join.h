/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/calendar_join.h
 *
 * PURPOSE:
 *   Compose multiple holiday calendars.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CALENDAR_JOIN_H
#define UMICOM_FINANCE_CORE_CALENDAR_JOIN_H

#include "umicom/finance/core/holiday_calendar.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCalendarJoinMode { UMI_CALENDAR_ALL_OPEN=0, UMI_CALENDAR_ANY_OPEN=1 } UmiCalendarJoinMode;
typedef struct UmiCalendarJoin { const UmiHolidayCalendar *items[8U]; size_t count; UmiCalendarJoinMode mode; } UmiCalendarJoin;
/* Initialize join. */ void umi_calendar_join_init(UmiCalendarJoin *j,UmiCalendarJoinMode mode);
/* Add calendar. */ UmiStatus umi_calendar_join_add(UmiCalendarJoin *j,const UmiHolidayCalendar *c);
/* Test joined business day. */ bool umi_calendar_join_is_business_day(const UmiCalendarJoin *j,UmiFinancialDate d);
#ifdef __cplusplus
}
#endif

#endif
