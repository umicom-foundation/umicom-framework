/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_model.c
 * PURPOSE: Check cash dates, bounds, atomic edits and independent end-of-day expectations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCashPlan *p = Plan();
    UmiCashPlanConfig c = Config();
    UmiCashPlanForecast *r = calloc(1U, sizeof *r);
    CHECK(r != NULL);
    UmiCashPlanEntry e = Entry("rent", 3, 85000, 0);
    if (strcmp(mode, "projection") == 0 || strcmp(mode, "order") == 0)
    {
        Add(p, "salary", 20, 50000, 1);
        Add(p, "rent", 3, 85000, 0);
        OK(UmiCashPlanProject(p, r));
        CHECK(r->day_count == 2U && r->days[0].date.day == 3U && r->days[1].date.day == 20U);
        CHECK(r->days[0].closing_minor == 15000 && r->days[0].headroom_minor == -5000 &&
              r->closing_minor == 65000);
        CHECK(r->minimum_minor == 15000 && r->minimum_date.day == 3U && r->has_shortfall &&
              r->first_shortfall_date.day == 3U);
        CHECK(r->total_inflow_minor == 50000 && r->total_outflow_minor == 85000);
    }
    else if (strcmp(mode, "empty") == 0)
    {
        OK(UmiCashPlanProject(p, r));
        CHECK(r->day_count == 0U && r->closing_minor == 100000 && r->minimum_minor == 100000 &&
              !r->has_shortfall);
    }
    else if (strcmp(mode, "copy") == 0)
    {
        OK(UmiCashPlanAdd(p, &e));
        UmiCashPlan *copy = NULL;
        OK(UmiCashPlanCopy(p, &copy));
        Same(p, copy);
        OK(UmiCashPlanRemove(p, 0U));
        CHECK(UmiCashPlanCount(copy) == 1U);
        UmiCashPlanDestroy(p);
        p = copy;
        OK(UmiCashPlanProject(p, r));
        CHECK(r->closing_minor == 15000);
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        OK(UmiCashPlanAdd(p, &e));
        uint64_t rev = UmiCashPlanRevision(p);
        CHECK(UmiCashPlanAdd(p, &e) == UMI_STATUS_ALREADY_EXISTS && UmiCashPlanRevision(p) == rev &&
              UmiCashPlanCount(p) == 1U);
    }
    else if (strcmp(mode, "replace") == 0)
    {
        OK(UmiCashPlanAdd(p, &e));
        Add(p, "other", 4, 10000, 1);
        uint64_t rev = UmiCashPlanRevision(p);
        CHECK(UmiCashPlanReplace(p, 1U, &e) == UMI_STATUS_ALREADY_EXISTS && UmiCashPlanRevision(p) == rev);
        e.amount.minor_units = 90000;
        OK(UmiCashPlanReplace(p, 0U, &e));
        OK(UmiCashPlanProject(p, r));
        CHECK(r->minimum_minor == 10000 && r->closing_minor == 20000);
    }
    else if (strcmp(mode, "remove") == 0)
    {
        OK(UmiCashPlanAdd(p, &e));
        Add(p, "other", 4, 10000, 1);
        OK(UmiCashPlanRemove(p, 0U));
        UmiCashPlanEntry at;
        OK(UmiCashPlanAt(p, 0U, &at));
        CHECK(strcmp(at.id, "other") == 0 && UmiCashPlanCount(p) == 1U);
        uint64_t rev = UmiCashPlanRevision(p);
        CHECK(UmiCashPlanRemove(p, 1U) == UMI_STATUS_NOT_FOUND && UmiCashPlanRevision(p) == rev);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        for (size_t i = 0U; i < UMI_CASH_PLAN_CAPACITY; ++i)
        {
            char id[48];
            (void)snprintf(id, sizeof id, "flow-%zu", i);
            Add(p, id, 3, 1, 1);
        }
        uint64_t rev = UmiCashPlanRevision(p);
        CHECK(UmiCashPlanAdd(p, &e) == UMI_STATUS_CAPACITY_EXCEEDED && UmiCashPlanRevision(p) == rev);
        OK(UmiCashPlanProject(p, r));
        CHECK(r->included == 128U && r->day_count == 1U && r->closing_minor == 100128);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        uint64_t rev = UmiCashPlanRevision(p);
        e.direction = (UmiFinancialDirection)99;
        CHECK(UmiCashPlanAdd(p, &e) != UMI_STATUS_OK);
        e = Entry("x", 3, -1, 1);
        CHECK(UmiCashPlanAdd(p, &e) != UMI_STATUS_OK);
        c.buffer_minor = -1;
        CHECK(UmiCashPlanSetConfig(p, &c) != UMI_STATUS_OK && UmiCashPlanRevision(p) == rev);
    }
    else if (strcmp(mode, "currency") == 0 || strcmp(mode, "scale") == 0)
    {
        OK(UmiCashPlanAdd(p, &e));
        uint64_t rev = UmiCashPlanRevision(p);
        if (strcmp(mode, "currency") == 0)
            memcpy(c.opening.currency.code, "USD", 4U);
        else
            c.opening.scale = 3U;
        CHECK(UmiCashPlanSetConfig(p, &c) == UMI_STATUS_INVALID_ARGUMENT && UmiCashPlanRevision(p) == rev);
        e.amount = c.opening;
        CHECK(UmiCashPlanReplace(p, 0U, &e) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "disabled") == 0)
    {
        e.enabled = 0;
        OK(UmiCashPlanAdd(p, &e));
        OK(UmiCashPlanProject(p, r));
        CHECK(r->disabled == 1U && r->included == 0U && r->closing_minor == 100000);
    }
    else if (strcmp(mode, "outside") == 0)
    {
        e.date = (UmiFinancialDate){2028, 1, 31};
        OK(UmiCashPlanAdd(p, &e));
        e = Entry("later", 1, 50, 1);
        e.date.month = 3;
        OK(UmiCashPlanAdd(p, &e));
        Add(p, "start", 1, 3, 1);
        Add(p, "end", 29, 1, 0);
        OK(UmiCashPlanProject(p, r));
        CHECK(r->before_start == 1U && r->after_end == 1U && r->included == 2U && r->closing_minor == 100002);
    }
    else if (strcmp(mode, "same-day") == 0)
    {
        Add(p, "pay", 3, 120000, 0);
        Add(p, "receive", 3, 120000, 1);
        OK(UmiCashPlanProject(p, r));
        CHECK(r->day_count == 1U && r->days[0].entries == 2U && r->closing_minor == 100000 &&
              !r->has_shortfall);
    }
    else if (strcmp(mode, "overflow") == 0 || strcmp(mode, "headroom-overflow") == 0 ||
             strcmp(mode, "total-overflow") == 0)
    {
        if (strcmp(mode, "headroom-overflow") == 0)
        {
            c.opening.minor_units = INT64_MIN;
            c.buffer_minor = 1;
            OK(UmiCashPlanSetConfig(p, &c));
        }
        else if (strcmp(mode, "total-overflow") == 0)
        {
            c.opening.minor_units = 0;
            c.buffer_minor = 0;
            OK(UmiCashPlanSetConfig(p, &c));
            Add(p, "in1", 2, INT64_MAX, 1);
            Add(p, "out1", 2, INT64_MAX, 0);
            Add(p, "in2", 3, 1, 1);
            Add(p, "out2", 3, 1, 0);
        }
        else
        {
            c.opening.minor_units = INT64_MAX;
            OK(UmiCashPlanSetConfig(p, &c));
            Add(p, "in", 2, 1, 1);
        }
        memset(r, 0x5a, sizeof *r);
        UmiCashPlanForecast *before = malloc(sizeof *before);
        CHECK(before != NULL);
        memcpy(before, r, sizeof *r);
        CHECK(UmiCashPlanProject(p, r) != UMI_STATUS_OK && memcmp(r, before, sizeof *r) == 0);
        free(before);
    }
    else if (strcmp(mode, "dates") == 0)
    {
        UmiFinancialDate date;
        char text[11];
        OK(UmiCashPlanDateParse("2000-02-29", 10U, &date));
        OK(UmiCashPlanDateFormat(date, text, sizeof text));
        CHECK(strcmp(text, "2000-02-29") == 0);
        CHECK(UmiCashPlanDateParse("1900-02-29", 10U, &date) != UMI_STATUS_OK);
        OK(UmiCashPlanDateParse("1600-01-01", 10U, &date));
        OK(UmiCashPlanDateParse("9999-12-31", 10U, &date));
    }
    else if (strcmp(mode, "date-refusal") == 0)
    {
        UmiFinancialDate date = {2020, 1, 1};
        CHECK(UmiCashPlanDateParse("2028-02-30", 10U, &date) != UMI_STATUS_OK && date.year == 2020);
        CHECK(UmiCashPlanDateParse("2028-2-01", 9U, &date) != UMI_STATUS_OK);
        char out[11] = "unchanged";
        CHECK(UmiCashPlanDateFormat((UmiFinancialDate){2028, 1, 1}, out, 10U) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              strcmp(out, "unchanged") == 0);
        c.end = c.start;
        c.end.year = 2027;
        CHECK(UmiCashPlanSetConfig(p, &c) != UMI_STATUS_OK);
    }
    else if (strcmp(mode, "utf8") == 0)
    {
        memcpy(e.label, "caf\xc3\xa9", 6U);
        OK(UmiCashPlanAdd(p, &e));
        e.id[0] = 'q';
        e.label[0] = (char)0xc0;
        e.label[1] = (char)0x80;
        e.label[2] = '\0';
        CHECK(UmiCashPlanAdd(p, &e) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "unterminated") == 0)
    {
        memset(e.id, 'x', sizeof e.id);
        CHECK(UmiCashPlanAdd(p, &e) != UMI_STATUS_OK);
        memset(c.title, 'x', sizeof c.title);
        CHECK(UmiCashPlanSetConfig(p, &c) != UMI_STATUS_OK);
    }
    else
        return 2;
    UmiCashPlanDestroy(p);
    free(r);
    return 0;
}
