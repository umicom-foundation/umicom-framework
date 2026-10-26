/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_period_calendar_boundaries.c
 *
 * PURPOSE:
 *   Exercise valid posting periods, inclusive date boundaries and refusal of ambiguous calendars.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/finance/accounting/accounting_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

/* Retain the wider fixture parameters for review. Match the public date fields
 * so month assignment cannot introduce an implicit narrowing warning. */
#if 0
static void Period(UmiAccountingAccountingPeriod *value, const char *id, int year, unsigned month)
#endif
static void Period(UmiAccountingAccountingPeriod *value, const char *id, int32_t year, uint8_t month)
{
    CHECK(umi_accounting_accounting_period_init(value, id,
        (UmiFinancialDate){year, month, 1U}, (UmiFinancialDate){year, month, 28U},
        UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiAccountingAccountingPeriod period;
    Period(&period, "period", 2026, 10U);
    if (!strncmp(mode, "calendar-", 9U)) {
        UmiAccountingAccountingCalendar *calendar = malloc(sizeof *calendar);
        UmiAccountingAccountingCalendar *saved = malloc(sizeof *saved);
        CHECK(calendar && saved);
        umi_accounting_accounting_calendar_init(calendar);
        CHECK(umi_accounting_accounting_calendar_add(calendar, &period) == UMI_STATUS_OK);
        if (!strcmp(mode, "calendar-find")) {
            CHECK(umi_accounting_accounting_calendar_find_date(calendar, period.start_date) == &calendar->items[0]);
            CHECK(umi_accounting_accounting_calendar_find_date(calendar, period.end_date) == &calendar->items[0]);
            CHECK(umi_accounting_accounting_calendar_find_date(calendar, (UmiFinancialDate){2026, 10U, 29U}) == NULL);
        } else if (!strcmp(mode, "calendar-null")) {
            CHECK(umi_accounting_accounting_calendar_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_accounting_accounting_calendar_find_date(NULL, period.start_date) == NULL);
            CHECK(umi_accounting_accounting_calendar_add(NULL, &period) == UMI_STATUS_INVALID_ARGUMENT);
        } else {
            if (!strcmp(mode, "calendar-count")) calendar->count = SIZE_MAX;
            else if (!strcmp(mode, "calendar-invalid-period")) calendar->items[0].start_date.day = 0U;
            else if (!strcmp(mode, "calendar-restored-overlap")) {
                calendar->items[1] = calendar->items[0];
                CHECK(umi_accounting_id_assign(&calendar->items[1].id, "overlap") == UMI_STATUS_OK);
                calendar->count = 2U;
            } else if (!strcmp(mode, "calendar-restored-id")) {
                Period(&calendar->items[1], "period", 2026, 11U);
                calendar->count = 2U;
            } else if (!strcmp(mode, "calendar-duplicate-id")) {
                Period(&period, "period", 2026, 11U);
            } else if (!strcmp(mode, "calendar-overlap")) {
                CHECK(umi_accounting_id_assign(&period.id, "overlap") == UMI_STATUS_OK);
                period.start_date = period.end_date;
            } else if (!strcmp(mode, "calendar-full")) {
                for (size_t i = 1U; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
                    char id[32];
                    int n = snprintf(id, sizeof id, "period-%zu", i);
                    CHECK(n > 0 && (size_t)n < sizeof id);
                    Period(&period, id, 2027 + (int)i, 1U);
                    CHECK(umi_accounting_accounting_calendar_add(calendar, &period) == UMI_STATUS_OK);
                }
                Period(&period, "extra", 2100, 1U);
            } else if (strcmp(mode, "calendar-null-period") && strcmp(mode, "calendar-alias")) return 2;
            memcpy(saved, calendar, sizeof *saved);
            const UmiAccountingAccountingPeriod *input = !strcmp(mode, "calendar-alias") ?
                &calendar->items[0] : !strcmp(mode, "calendar-null-period") ? NULL : &period;
            CHECK(umi_accounting_accounting_calendar_add(calendar, input) != UMI_STATUS_OK);
            CHECK(memcmp(saved, calendar, sizeof *saved) == 0);
            if (!strcmp(mode, "calendar-count") || !strcmp(mode, "calendar-invalid-period") ||
                !strcmp(mode, "calendar-restored-overlap") || !strcmp(mode, "calendar-restored-id")) {
                CHECK(umi_accounting_accounting_calendar_validate(calendar) != UMI_STATUS_OK);
                CHECK(umi_accounting_accounting_calendar_find_date(calendar, period.start_date) == NULL);
            }
        }
        free(saved);
        free(calendar);
        return 0;
    }
    if (!strcmp(mode, "init-alias") || !strcmp(mode, "init-atomic")) {
        UmiAccountingAccountingPeriod saved;
        memcpy(&saved, &period, sizeof saved);
        UmiStatus status = umi_accounting_accounting_period_init(&period, period.id.value,
            period.start_date, period.end_date, !strcmp(mode, "init-atomic") ?
            (UmiAccountingPeriodStatus)99 : UMI_ACCOUNTING_PERIOD_CLOSED);
        if (!strcmp(mode, "init-atomic")) CHECK(status != UMI_STATUS_OK && memcmp(&saved, &period, sizeof saved) == 0);
        else CHECK(status == UMI_STATUS_OK && !strcmp(saved.id.value, period.id.value) &&
            !umi_accounting_accounting_period_open(&period));
        return 0;
    }
    if (!strcmp(mode, "inclusive") || !strcmp(mode, "closed") || !strcmp(mode, "soft-closed")) {
        if (!strcmp(mode, "closed")) period.status = UMI_ACCOUNTING_PERIOD_CLOSED;
        if (!strcmp(mode, "soft-closed")) period.status = UMI_ACCOUNTING_PERIOD_SOFT_CLOSED;
        CHECK(umi_accounting_accounting_period_valid(&period));
        CHECK(umi_accounting_accounting_period_open(&period) == (!strcmp(mode, "inclusive")));
        CHECK(umi_accounting_accounting_period_contains(&period, period.start_date));
        CHECK(umi_accounting_accounting_period_contains(&period, period.end_date));
        CHECK(!umi_accounting_accounting_period_contains(&period, (UmiFinancialDate){2026, 9U, 30U}));
        CHECK(!umi_accounting_accounting_period_contains(&period, (UmiFinancialDate){2026, 10U, 29U}));
        CHECK(!umi_accounting_accounting_period_contains(&period, (UmiFinancialDate){2026, 10U, 0U}));
        return 0;
    }
    if (!strcmp(mode, "id")) period.id.value[0] = '\0';
    else if (!strcmp(mode, "id-unterminated")) memset(period.id.value, 'x', sizeof period.id.value);
    else if (!strcmp(mode, "reversed")) period.start_date = (UmiFinancialDate){2026, 11U, 1U};
    else if (!strcmp(mode, "date")) period.end_date.day = 0U;
    else if (!strcmp(mode, "status")) period.status = (UmiAccountingPeriodStatus)99;
    else if (!strcmp(mode, "negative-status")) period.status = (UmiAccountingPeriodStatus)-1;
    else if (!strcmp(mode, "null")) {
        CHECK(!umi_accounting_accounting_period_valid(NULL));
        CHECK(!umi_accounting_accounting_period_open(NULL));
        CHECK(!umi_accounting_accounting_period_contains(NULL, period.start_date));
        return 0;
    } else return 2;
    CHECK(!umi_accounting_accounting_period_valid(&period));
    CHECK(!umi_accounting_accounting_period_open(&period));
    CHECK(!umi_accounting_accounting_period_contains(&period, period.start_date));
    return 0;
}
