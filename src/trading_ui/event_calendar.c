/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/event_calendar.c
 *
 * PURPOSE:
 *   Validate event dates and filter immutable snapshots without changing trading state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "event_calendar_internal.h"
#include <stdlib.h>
#include <string.h>
static bool Terminated(const char *text, size_t capacity) {
    return memchr(text, '\0', capacity) != NULL;
}
static bool Digits(const char *s, size_t n) {
    for (size_t i = 0U; i < n; ++i)
        if (s[i] < '0' || s[i] > '9')
            return false;
    return true;
}
static unsigned Pair(const char *s) {
    return (unsigned)(s[0] - '0') * 10U + (unsigned)(s[1] - '0');
}
bool UmiEventCalendarDateValid(const char *date, bool empty) {
    if (date == NULL)
        return false;
    if (date[0] == '\0')
        return empty;
    if (strlen(date) != 10U || date[4] != '-' || date[7] != '-' || !Digits(date, 4U) ||
        !Digits(date + 5, 2U) || !Digits(date + 8, 2U))
        return false;
    unsigned year = Pair(date) * 100U + Pair(date + 2), month = Pair(date + 5),
             day = Pair(date + 8);
    if (year == 0U || month == 0U || month > 12U)
        return false;
    static const unsigned days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned limit = days[month - 1U];
    if (month == 2U && year % 4U == 0U && (year % 100U != 0U || year % 400U == 0U))
        ++limit;
    return day > 0U && day <= limit;
}
static bool TimeValid(const char *time) {
    return strlen(time) == 5U && time[2] == ':' && Digits(time, 2U) && Digits(time + 3, 2U) &&
           Pair(time) < 24U && Pair(time + 3) < 60U;
}
static bool ObservedValid(const char *stamp) {
    if (strlen(stamp) != 20U || stamp[10] != 'T' || stamp[16] != ':' || stamp[19] != 'Z')
        return false;
    char date[11], time[6];
    memcpy(date, stamp, 10U);
    date[10] = '\0';
    memcpy(time, stamp + 11, 5U);
    time[5] = '\0';
    return UmiEventCalendarDateValid(date, false) && TimeValid(time) && Digits(stamp + 17, 2U) &&
           Pair(stamp + 17) < 60U;
}
static bool CodeValid(const char *code, size_t width) {
    if (code[0] == '\0')
        return true;
    if (strlen(code) != width)
        return false;
    for (size_t i = 0U; i < width; ++i)
        if (code[i] < 'A' || code[i] > 'Z')
            return false;
    return true;
}
unsigned UmiEventCalendarImportance(const UmiCalendarEvent *e) {
    return strcmp(e->importance, "high") == 0     ? 3U
           : strcmp(e->importance, "medium") == 0 ? 2U
           : strcmp(e->importance, "low") == 0    ? 1U
                                                  : 0U;
}
bool UmiEventCalendarEventValid(const UmiCalendarEvent *e) {
    /* CSV decoding establishes bounded termination and UTF-8 before these
     * semantic checks. Unknown timing must never become a midnight event. */
    if (!UmiEventCalendarDateValid(e->date, false) || !ObservedValid(e->observed_at) ||
        !CodeValid(e->country, 2U) || !CodeValid(e->currency, 3U) || !e->id[0] || !e->source[0] ||
        !e->title[0] || !e->timezone[0])
        return false;
    bool earnings = strcmp(e->kind, "earnings") == 0;
    if (!earnings && strcmp(e->kind, "economic") != 0)
        return false;
    if (earnings && (!e->symbol[0] || !e->venue[0]))
        return false;
    if (strcmp(e->timing, "exact") == 0) {
        if (!TimeValid(e->time))
            return false;
    } else if (e->time[0] ||
               (strcmp(e->timing, "pre-market") != 0 && strcmp(e->timing, "after-hours") != 0 &&
                strcmp(e->timing, "unspecified") != 0))
        return false;
    if (strcmp(e->status, "scheduled") != 0 && strcmp(e->status, "reported") != 0 &&
        strcmp(e->status, "cancelled") != 0)
        return false;
    if (strcmp(e->importance, "unknown") != 0 && UmiEventCalendarImportance(e) == 0U)
        return false;
    if (strcmp(e->quality, "example") != 0 && strcmp(e->quality, "imported") != 0)
        return false;
    if ((e->estimate[0] || e->actual[0] || e->previous[0]) && !e->unit[0])
        return false;
    return true;
}
void UmiEventCalendarDestroy(UmiEventCalendar *calendar) {
    if (calendar != NULL) {
        free(calendar->rows);
        free(calendar);
    }
}
size_t UmiEventCalendarCount(const UmiEventCalendar *calendar) {
    return calendar != NULL ? calendar->count : 0U;
}
const UmiCalendarEvent *UmiEventCalendarAt(const UmiEventCalendar *calendar, size_t index) {
    return calendar != NULL && index < calendar->count ? calendar->rows + index : NULL;
}
UmiStatus UmiEventCalendarFilterValidate(const UmiEventCalendarFilter *f) {
    if (f == NULL || f->kind < UMI_EVENT_ALL || f->kind > UMI_EVENT_EARNINGS ||
        f->minimum_importance > 3U || !Terminated(f->from, sizeof f->from) ||
        !Terminated(f->through, sizeof f->through) || !Terminated(f->search, sizeof f->search) ||
        !Terminated(f->country, sizeof f->country) || !Terminated(f->currency, sizeof f->currency))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiEventCalendarDateValid(f->from, true) || !UmiEventCalendarDateValid(f->through, true) ||
        (f->from[0] && f->through[0] && strcmp(f->from, f->through) > 0) ||
        !CodeValid(f->country, 2U) || !CodeValid(f->currency, 3U))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; f->search[i]; ++i)
        if ((unsigned char)f->search[i] < 32U || (unsigned char)f->search[i] == 127U)
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static unsigned char Fold(unsigned char c) {
    return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c;
}
static bool Contains(const char *text, const char *query) {
    if (!query[0])
        return true;
    for (size_t i = 0U; text[i]; ++i) {
        size_t n = 0U;
        while (query[n] && text[i + n] &&
               Fold((unsigned char)query[n]) == Fold((unsigned char)text[i + n]))
            ++n;
        if (!query[n])
            return true;
    }
    return false;
}
static bool Matches(const UmiCalendarEvent *e, const UmiEventCalendarFilter *f) {
    if (f->kind == UMI_EVENT_ECONOMIC && strcmp(e->kind, "economic") != 0)
        return false;
    if (f->kind == UMI_EVENT_EARNINGS && strcmp(e->kind, "earnings") != 0)
        return false;
    return (!f->from[0] || strcmp(e->date, f->from) >= 0) &&
           (!f->through[0] || strcmp(e->date, f->through) <= 0) &&
           (!f->country[0] || strcmp(e->country, f->country) == 0) &&
           (!f->currency[0] || strcmp(e->currency, f->currency) == 0) &&
           UmiEventCalendarImportance(e) >= f->minimum_importance &&
           (Contains(e->title, f->search) || Contains(e->symbol, f->search) ||
            Contains(e->venue, f->search) || Contains(e->source, f->search));
}
UmiStatus UmiEventCalendarSelect(const UmiEventCalendar *calendar,
                                 const UmiEventCalendarFilter *filter, size_t offset,
                                 size_t *indexes, size_t capacity, size_t *written, size_t *total) {
    if (calendar == NULL || written == NULL || total == NULL || written == total ||
        (indexes == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiEventCalendarFilterValidate(filter);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = 0U, used = 0U;
    for (size_t i = 0U; i < calendar->count; ++i)
        if (Matches(calendar->rows + i, filter)) {
            if (count >= offset && used < capacity)
                indexes[used++] = i;
            ++count;
        }
    *written = used;
    *total = count;
    return UMI_STATUS_OK;
}
