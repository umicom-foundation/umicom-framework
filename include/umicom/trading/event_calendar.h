/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/event_calendar.h
 *
 * PURPOSE:
 *   Own bounded economic and earnings calendars independently of trading execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_EVENT_CALENDAR_H
#define UMICOM_TRADING_EVENT_CALENDAR_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EVENT_CALENDAR_MAX_ROWS 4096U
#define UMI_EVENT_CALENDAR_MAX_BYTES (4U * 1024U * 1024U)
typedef enum UmiEventKind { UMI_EVENT_ALL, UMI_EVENT_ECONOMIC, UMI_EVENT_EARNINGS } UmiEventKind;
typedef struct UmiEventCalendar UmiEventCalendar;
/* All strings are owned, bounded UTF-8. Dates and times retain the provider's
 * timezone label; neither a date nor a session label implies a UTC instant.
 * Numeric-looking values remain text with a unit. Empty means not supplied,
 * including missing estimates, actuals and previous-period values. */
typedef struct UmiCalendarEvent {
    char id[96], kind[16], date[11], time[6], timezone[64], timing[20];
    char status[16], country[3], currency[4], symbol[48], venue[48];
    char title[192], period[64], importance[8];
    char estimate[64], actual[64], previous[64], unit[32];
    char source[128], observed_at[21], quality[16];
} UmiCalendarEvent;
typedef struct UmiEventCalendarFilter {
    UmiEventKind kind;
    char from[11], through[11], search[128], country[3], currency[4];
    unsigned minimum_importance; /* 0=all; 1=low; 2=medium; 3=high */
} UmiEventCalendarFilter;
/* Parse the exact documented 21-column CSV schema into an immutable snapshot.
 * Uses the shared UTF-8 CSV reader; duplicate (source,id) identities, invalid
 * dates/times, unknown enums, overlong fields and controls are refused.
 * Header-only is a valid empty snapshot. No network or broker is accessed.
 * On failure *out is NULL; error_record is a logical CSV record, or 0 if the
 * input could not be read. Input is copied; use and destroy on one owner thread. */
UmiStatus UmiEventCalendarReadCsv(const void *bytes, size_t length, UmiEventCalendar **out,
                                  size_t *error_record);
void UmiEventCalendarDestroy(UmiEventCalendar *calendar);
size_t UmiEventCalendarCount(const UmiEventCalendar *calendar);
/* A borrowed row, stable until Destroy; NULL for an invalid index. Sorted by
 * provider date, exact local time (unknown last), source and ID. Mixed zones
 * are not a global chronological order. */
const UmiCalendarEvent *UmiEventCalendarAt(const UmiEventCalendar *calendar, size_t index);
/* Blank dates/country/currency/search impose no restriction. Search is an
 * ASCII-case-insensitive substring over title, symbol, venue and source;
 * non-ASCII bytes compare exactly. Country/currency are uppercase codes.
 * Checked filters never infer an instrument identity or trading permission. */
UmiStatus UmiEventCalendarFilterValidate(const UmiEventCalendarFilter *filter);
/* Page over matching rows without partial publication. total counts all
 * matches, written the current page. NULL indexes requires capacity 0.
 * Empty page/offset beyond the end succeeds. Output scalars must be distinct;
 * output storage and the filter must not overlap. Failure preserves outputs. */
UmiStatus UmiEventCalendarSelect(const UmiEventCalendar *calendar,
                                 const UmiEventCalendarFilter *filter, size_t offset,
                                 size_t *indexes, size_t capacity, size_t *written, size_t *total);
#ifdef __cplusplus
}
#endif
#endif
