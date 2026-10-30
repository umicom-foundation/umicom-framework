/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/event_calendar/calendar_fixture.h
 *
 * PURPOSE:
 *   Supply fictional dated events for calendar parsing and native view regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EVENT_CALENDAR_FIXTURE_H
#define UMICOM_EVENT_CALENDAR_FIXTURE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
#define CALENDAR_HEADER                                                                            \
    "id,kind,date,time,timezone,timing,status,country,currency,symbol,venue,title,period,"         \
    "importance,estimate,actual,previous,unit,source,observed_at,quality\n"
#define ECONOMIC_ROW                                                                               \
    "release,economic,2026-11-02,09:30,UTC,exact,scheduled,GB,GBP,,,Example employment "           \
    "release,2026-10,high,1.2,,1.0,percent,Example statistics,2026-10-30T08:00:00Z,example\n"
#define EARNINGS_ROW                                                                               \
    "earnings,earnings,2026-11-02,,America/"                                                       \
    "New_York,after-hours,reported,US,USD,EXAMPLE,XNAS,Example company,Q3 "                        \
    "2026,medium,2.0,2.5,,USD per diluted share,Example company "                                  \
    "calendar,2026-10-30T08:00:00Z,example\n"
#define CALENDAR_CSV CALENDAR_HEADER EARNINGS_ROW ECONOMIC_ROW
/* Replace one known token in an owned test input; never edit production data. */
static inline char *CalendarReplace(const char *text, const char *token, const char *replacement) {
    const char *found = strstr(text, token);
    CHECK(found != NULL);
    size_t prefix = (size_t)(found - text),
           size = strlen(text) - strlen(token) + strlen(replacement) + 1U;
    char *out = malloc(size);
    CHECK(out != NULL);
    memcpy(out, text, prefix);
    memcpy(out + prefix, replacement, strlen(replacement));
    strcpy(out + prefix + strlen(replacement), found + strlen(token));
    return out;
}
#endif
