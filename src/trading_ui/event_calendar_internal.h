/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/event_calendar_internal.h
 *
 * PURPOSE:
 *   Share checked calendar schema helpers without exposing mutable snapshot storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EVENT_CALENDAR_INTERNAL_H
#define UMICOM_EVENT_CALENDAR_INTERNAL_H
#include "umicom/trading/event_calendar.h"
struct UmiEventCalendar {
    UmiCalendarEvent *rows;
    size_t count;
};
bool UmiEventCalendarDateValid(const char *date, bool empty);
bool UmiEventCalendarEventValid(const UmiCalendarEvent *event);
unsigned UmiEventCalendarImportance(const UmiCalendarEvent *event);
#endif
