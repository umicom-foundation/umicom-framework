/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_comparison.c
 * PURPOSE: Check economic differences, stable identities and all-or-nothing comparison failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/cash_planning/comparison.h"

static UmiStatus CompareUnchanged(UmiCashPlan *a, UmiCashPlan *b, UmiCashPlanComparison *out)
{
    size_t an, bn, afterSize;
    char *beforeA = Encode(a, &an), *beforeB = Encode(b, &bn);
    uint64_t ar = UmiCashPlanRevision(a), br = UmiCashPlanRevision(b);
    UmiStatus status = UmiCashPlanCompare(a, b, out);
    char *after = Encode(a, &afterSize);
    CHECK(afterSize == an && memcmp(beforeA, after, an) == 0 && UmiCashPlanRevision(a) == ar);
    UmiCashPlanFreeBytes(after);
    after = Encode(b, &afterSize);
    CHECK(afterSize == bn && memcmp(beforeB, after, bn) == 0 && UmiCashPlanRevision(b) == br);
    UmiCashPlanFreeBytes(after);
    UmiCashPlanFreeBytes(beforeA);
    UmiCashPlanFreeBytes(beforeB);
    return status;
}
static void Refused(UmiCashPlan *a, UmiCashPlan *b, UmiCashPlanComparison *out, UmiStatus expected)
{
    unsigned char *before = malloc(sizeof *out);
    CHECK(before != NULL);
    memset(out, 0xa5, sizeof *out);
    memcpy(before, out, sizeof *out);
    CHECK(CompareUnchanged(a, b, out) == expected);
    CHECK(memcmp(before, out, sizeof *out) == 0);
    free(before);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCashPlan *a = Plan(), *b = Plan();
    UmiCashPlanComparison *report = calloc(1U, sizeof *report);
    CHECK(report != NULL);
    if (strcmp(mode, "identical") == 0)
    {
        Add(a, "rent", 3, 85000, 0);
        OK(CompareUnchanged(a, a, report));
        CHECK(report->unchanged == 1U && report->changed == 0U && report->added == 0U && report->removed == 0U);
        CHECK(report->closing_delta_minor == 0 && report->minimum_headroom_delta_minor == 0);
    }
    else if (strcmp(mode, "balances") == 0)
    {
        Add(a, "rent", 3, 85000, 0);
        UmiCashPlanConfig c = Config();
        c.opening.minor_units = 120000;
        c.buffer_minor = 50000;
        OK(UmiCashPlanSetConfig(b, &c));
        Add(b, "rent", 3, 90000, 0);
        Add(b, "deposit", 1, 10000, 1);
        OK(CompareUnchanged(a, b, report));
        CHECK(report->opening_delta_minor == 20000 && report->buffer_delta_minor == 30000);
        CHECK(report->inflow_delta_minor == 10000 && report->outflow_delta_minor == 5000);
        CHECK(report->closing_delta_minor == 25000 && report->minimum_delta_minor == 25000);
        CHECK(report->minimum_headroom_delta_minor == -5000);
        CHECK(report->baseline.closing_minor == 15000 && report->scenario.closing_minor == 40000);
        CHECK(report->baseline.has_shortfall && report->scenario.has_shortfall);
        CHECK(report->scenario.first_shortfall_date.day == 3U);
        CHECK(report->changed == 1U && report->added == 1U);
    }
    else if (strcmp(mode, "stable-ids") == 0)
    {
        Add(a, "old", 2, 100, 0); Add(a, "keep", 3, 200, 1); Add(a, "edit", 4, 300, 0);
        Add(b, "edit", 4, 400, 0); Add(b, "new", 2, 100, 0); Add(b, "keep", 3, 200, 1);
        OK(CompareUnchanged(a, b, report));
        CHECK(report->removed == 1U && report->added == 1U && report->changed == 1U && report->unchanged == 1U);
        CHECK(report->closing_delta_minor == -100);
    }
    else if (strcmp(mode, "reordered") == 0)
    {
        Add(a, "one", 2, 100, 0); Add(a, "two", 3, 200, 1);
        Add(b, "two", 3, 200, 1); Add(b, "one", 2, 100, 0);
        OK(CompareUnchanged(a, b, report));
        CHECK(report->unchanged == 2U && report->changed == 0U && report->closing_delta_minor == 0);
    }
    else if (strcmp(mode, "label") == 0 || strcmp(mode, "date") == 0 || strcmp(mode, "amount") == 0 ||
             strcmp(mode, "direction") == 0 || strcmp(mode, "enabled") == 0)
    {
        UmiCashPlanEntry entry = Entry("rent", 3, 85000, 0);
        OK(UmiCashPlanAdd(a, &entry));
        if (strcmp(mode, "label") == 0) (void)snprintf(entry.label, sizeof entry.label, "Revised rent");
        if (strcmp(mode, "date") == 0) entry.date.day = 10U;
        if (strcmp(mode, "amount") == 0) entry.amount.minor_units = 90000;
        if (strcmp(mode, "direction") == 0) entry.direction = UMI_FINANCIAL_DIRECTION_RECEIVE;
        if (strcmp(mode, "enabled") == 0) entry.enabled = 0;
        OK(UmiCashPlanAdd(b, &entry));
        OK(CompareUnchanged(a, b, report));
        CHECK(report->changed == 1U && report->unchanged == 0U && report->added == 0U && report->removed == 0U);
        if (strcmp(mode, "date") == 0)
            CHECK(report->baseline.minimum_date.day == 3U && report->scenario.minimum_date.day == 10U);
        if (strcmp(mode, "enabled") == 0)
            CHECK(report->scenario.disabled == 1U && report->closing_delta_minor == 85000);
    }
    else if (strcmp(mode, "excluded") == 0)
    {
        UmiCashPlanEntry entry = Entry("disabled", 3, 100, 0);
        entry.enabled = 0;
        OK(UmiCashPlanAdd(a, &entry)); entry.amount.minor_units = 200; OK(UmiCashPlanAdd(b, &entry));
        entry = Entry("outside", 3, 300, 0); entry.date.month = 3U;
        OK(UmiCashPlanAdd(a, &entry)); entry.amount.minor_units = 400; OK(UmiCashPlanAdd(b, &entry));
        OK(CompareUnchanged(a, b, report));
        CHECK(report->changed == 2U && report->closing_delta_minor == 0);
        CHECK(report->scenario.disabled == 1U && report->scenario.after_end == 1U && report->scenario.included == 0U);
    }
    else if (strcmp(mode, "title") == 0)
    {
        UmiCashPlanConfig c = Config();
        (void)snprintf(c.title, sizeof c.title, "Revised scenario");
        OK(UmiCashPlanSetConfig(b, &c)); OK(CompareUnchanged(a, b, report));
        CHECK(report->changed == 0U && report->closing_delta_minor == 0);
        CHECK(strcmp(report->scenario.config.title, "Revised scenario") == 0);
    }
    else if (strcmp(mode, "currency") == 0 || strcmp(mode, "scale") == 0 ||
             strcmp(mode, "start") == 0 || strcmp(mode, "end") == 0)
    {
        UmiCashPlanConfig c = Config();
        if (strcmp(mode, "currency") == 0) memcpy(c.opening.currency.code, "USD", 4U);
        if (strcmp(mode, "scale") == 0) c.opening.scale = 3U;
        if (strcmp(mode, "start") == 0) c.start.day = 2U;
        if (strcmp(mode, "end") == 0) c.end.day = 28U;
        OK(UmiCashPlanSetConfig(b, &c)); Refused(a, b, report, UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "projection-overflow") == 0)
    {
        Add(b, "large", 2, INT64_MAX, 1);
        Refused(a, b, report, UMI_STATUS_CAPACITY_EXCEEDED);
        Refused(b, a, report, UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "delta-overflow") == 0 || strcmp(mode, "headroom-overflow") == 0)
    {
        UmiCashPlanConfig ca = Config(), cb = Config();
        ca.buffer_minor = cb.buffer_minor = 0;
        if (strcmp(mode, "delta-overflow") == 0)
        { ca.opening.minor_units = INT64_MIN; cb.opening.minor_units = 0; }
        else
        { ca.opening.minor_units = -1; ca.buffer_minor = INT64_MAX; cb.opening.minor_units = 0; }
        OK(UmiCashPlanSetConfig(a, &ca)); OK(UmiCashPlanSetConfig(b, &cb));
        UmiCashPlanForecast *individual = calloc(1U, sizeof *individual); CHECK(individual != NULL);
        OK(UmiCashPlanProject(a, individual)); OK(UmiCashPlanProject(b, individual)); free(individual);
        Refused(a, b, report, UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        for (size_t i = 0U; i < UMI_CASH_PLAN_CAPACITY; ++i)
        {
            char id[48]; (void)snprintf(id, sizeof id, "entry-%zu", i);
            Add(a, id, 2, 1, 0); Add(b, id, 2, 1, 0);
        }
        OK(CompareUnchanged(a, b, report));
        CHECK(report->unchanged == UMI_CASH_PLAN_CAPACITY && report->closing_delta_minor == 0);
    }
    else if (strcmp(mode, "null") == 0)
    {
        memset(report, 0xa5, sizeof *report);
        unsigned char *before = malloc(sizeof *report); CHECK(before != NULL);
        memcpy(before, report, sizeof *report);
        CHECK(UmiCashPlanCompare(NULL, b, report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCashPlanCompare(a, NULL, report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCashPlanCompare(a, b, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, report, sizeof *report) == 0); free(before);
    }
    else
        return 2;
    free(report);
    UmiCashPlanDestroy(a); UmiCashPlanDestroy(b);
    return 0;
}
