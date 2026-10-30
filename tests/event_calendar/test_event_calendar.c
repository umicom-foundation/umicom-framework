/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/event_calendar/test_event_calendar.c
 *
 * PURPOSE:
 *   Exercise calendar schema, event identity, source-time semantics and bounded queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "calendar_fixture.h"
#include "umicom/trading/event_calendar.h"
static void Refuse(const char *token, const char *replacement, UmiStatus expected) {
    char *input = CalendarReplace(CALENDAR_CSV, token, replacement);
    UmiEventCalendar *calendar = NULL;
    size_t record = 0U;
    CHECK(UmiEventCalendarReadCsv(input, strlen(input), &calendar, &record) == expected);
    CHECK(calendar == NULL && record <= 3U);
    free(input);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiEventCalendar *c = NULL;
    size_t record = 99U;
    CHECK(UmiEventCalendarReadCsv(CALENDAR_CSV, strlen(CALENDAR_CSV), &c, &record) ==
              UMI_STATUS_OK &&
          record == 0U);
    CHECK(UmiEventCalendarCount(c) == 2U);
    const UmiCalendarEvent *first = UmiEventCalendarAt(c, 0U);
    CHECK(first != NULL);
    UmiEventCalendarFilter f = {0};
    size_t indexes[4] = {99, 99, 99, 99}, written = 9U, total = 9U;
    if (strcmp(test, "ordering") == 0) {
        CHECK(strcmp(first->id, "release") == 0);
        CHECK(UmiEventCalendarAt(c, 2U) == NULL);
    } else if (strcmp(test, "missing-values") == 0) {
        CHECK(first->actual[0] == '\0');
        CHECK(strcmp(first->previous, "1.0") == 0);
        CHECK(UmiEventCalendarAt(c, 1U)->time[0] == '\0');
    } else if (strcmp(test, "earnings") == 0) {
        f.kind = UMI_EVENT_EARNINGS;
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 1U && written == 1U);
        CHECK(strcmp(UmiEventCalendarAt(c, indexes[0])->symbol, "EXAMPLE") == 0);
    } else if (strcmp(test, "date-range") == 0) {
        strcpy(f.from, "2026-11-03");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 0U);
        strcpy(f.from, "2026-11-02");
        strcpy(f.through, "2026-11-02");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 2U);
    } else if (strcmp(test, "country-currency") == 0) {
        strcpy(f.country, "GB");
        strcpy(f.currency, "GBP");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 1U);
        strcpy(f.currency, "USD");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 0U);
    } else if (strcmp(test, "search") == 0) {
        strcpy(f.search, "eXaMpLe CoMpAnY");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 1U);
    } else if (strcmp(test, "importance") == 0) {
        f.minimum_importance = 3U;
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) == UMI_STATUS_OK &&
              total == 1U);
    } else if (strcmp(test, "pages") == 0) {
        CHECK(UmiEventCalendarSelect(c, &f, 1U, indexes, 1U, &written, &total) == UMI_STATUS_OK &&
              total == 2U && written == 1U && indexes[0] == 1U);
        CHECK(UmiEventCalendarSelect(c, &f, (size_t)-1, indexes, 1U, &written, &total) ==
                  UMI_STATUS_OK &&
              written == 0U && total == 2U);
        CHECK(UmiEventCalendarSelect(c, &f, 0U, NULL, 0U, &written, &total) == UMI_STATUS_OK &&
              written == 0U && total == 2U);
    } else if (strcmp(test, "invalid-filter") == 0) {
        strcpy(f.from, "2026-11-03");
        strcpy(f.through, "2026-11-02");
        CHECK(UmiEventCalendarSelect(c, &f, 0U, indexes, 4U, &written, &total) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              written == 9U && total == 9U && indexes[0] == 99U);
        memset(&f, 0, sizeof f);
        memset(f.search, 'x', sizeof f.search);
        CHECK(UmiEventCalendarFilterValidate(&f) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "leap-date") == 0) {
        char *input = CalendarReplace(CALENDAR_CSV, "2026-11-02", "2028-02-29");
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(input, strlen(input), &next, NULL) == UMI_STATUS_OK);
        UmiEventCalendarDestroy(next);
        free(input);
        Refuse("2026-11-02", "2100-02-29", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "invalid-time") == 0) {
        Refuse("09:30", "24:01", UMI_STATUS_PARSE_ERROR);
        Refuse(",,America/New_York", ",16:00,America/New_York", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "observed-at") == 0) {
        Refuse("2026-10-30T08:00:00Z", "2026-10-30T08:00:60Z", UMI_STATUS_PARSE_ERROR);
        Refuse("2026-10-30T08:00:00Z", "2026-10-30T08:00:00", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "identity") == 0) {
        Refuse(",EXAMPLE,XNAS,", ",EXAMPLE,,", UMI_STATUS_PARSE_ERROR);
        Refuse("Example statistics", "", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "units") == 0) {
        Refuse(",percent,", ",,", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "unknown-enum") == 0) {
        Refuse(",reported,", ",finished,", UMI_STATUS_PARSE_ERROR);
        Refuse(",after-hours,", ",midnight,", UMI_STATUS_PARSE_ERROR);
        Refuse(",example\n", ",live\n", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "duplicate") == 0) {
        const char *input = CALENDAR_HEADER ECONOMIC_ROW ECONOMIC_ROW;
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(input, strlen(input), &next, &record) ==
                  UMI_STATUS_ALREADY_EXISTS &&
              record == 3U && next == NULL);
    } else if (strcmp(test, "source-identity") == 0) {
        const char *input = CALENDAR_HEADER ECONOMIC_ROW ECONOMIC_ROW;
        char *changed = CalendarReplace(input, "Example statistics", "Other source");
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(changed, strlen(changed), &next, NULL) == UMI_STATUS_OK &&
              UmiEventCalendarCount(next) == 2U);
        free(changed);
        UmiEventCalendarDestroy(next);
    } else if (strcmp(test, "schema") == 0) {
        Refuse("id,kind", "identity,kind", UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "quoted-title") == 0) {
        char *input =
            CalendarReplace(CALENDAR_CSV, "Example company,Q3", "\"Example, \"\"company\"\"\",Q3");
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(input, strlen(input), &next, NULL) == UMI_STATUS_OK);
        CHECK(strcmp(UmiEventCalendarAt(next, 1U)->title, "Example, \"company\"") == 0);
        free(input);
        UmiEventCalendarDestroy(next);
    } else if (strcmp(test, "control-text") == 0) {
        Refuse("Example company,Q3", "\"Example\ncompany\",Q3", UMI_STATUS_PARSE_ERROR);
        Refuse("Example company,Q3",
               "Example\x01"
               "company,Q3",
               UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "overlong") == 0) {
        char title[250];
        memset(title, 'a', sizeof title - 1U);
        title[sizeof title - 1U] = '\0';
        Refuse("Example company", title, UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(test, "header-only") == 0) {
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(CALENDAR_HEADER, strlen(CALENDAR_HEADER), &next, NULL) ==
                  UMI_STATUS_OK &&
              UmiEventCalendarCount(next) == 0U);
        UmiEventCalendarDestroy(next);
        CHECK(UmiEventCalendarReadCsv(NULL, 0U, &next, NULL) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(test, "byte-bound") == 0) {
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(NULL, UMI_EVENT_CALENDAR_MAX_BYTES + 1U, &next, NULL) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              next == NULL);
    } else if (strcmp(test, "owned-copy") == 0) {
        char *input = CalendarReplace(CALENDAR_CSV, "Example company", "Owned company");
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(input, strlen(input), &next, NULL) == UMI_STATUS_OK);
        memset(input, 'x', strlen(input));
        free(input);
        CHECK(strcmp(UmiEventCalendarAt(next, 1U)->title, "Owned company") == 0);
        UmiEventCalendarDestroy(next);
    } else if (strcmp(test, "row-bound") == 0) {
        size_t capacity = 2U * 1024U * 1024U;
        char *input = malloc(capacity);
        CHECK(input != NULL);
        strcpy(input, CALENDAR_HEADER);
        size_t used = strlen(input);
        for (size_t i = 0U; i <= UMI_EVENT_CALENDAR_MAX_ROWS; ++i) {
            int n = snprintf(input + used, capacity - used, "r%zu%s", i, strchr(ECONOMIC_ROW, ','));
            CHECK(n > 0 && (size_t)n < capacity - used);
            used += (size_t)n;
        }
        UmiEventCalendar *next = NULL;
        CHECK(UmiEventCalendarReadCsv(input, used, &next, NULL) == UMI_STATUS_CAPACITY_EXCEEDED &&
              next == NULL);
        free(input);
    } else
        CHECK(0);
    UmiEventCalendarDestroy(c);
    return 0;
}
