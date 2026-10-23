/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/comparison.c
 * PURPOSE: Publish complete scenario differences without changing either cash plan.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "plan_private.h"
#include "umicom/cash_planning/comparison.h"
#include "umicom/finance/money.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus Difference(UmiMoney unit, int64_t scenario, int64_t baseline, int64_t *out)
{
    UmiMoney left = unit, right = unit, result;
    left.minor_units = scenario;
    right.minor_units = baseline;
    UmiStatus status = umi_money_subtract(&left, &right, &result);
    if (status == UMI_STATUS_OK)
        *out = result.minor_units;
    return status;
}
static const UmiCashPlanEntry *Find(const UmiCashPlan *plan, const char *id)
{
    for (size_t i = 0U; i < plan->count; ++i)
        if (strcmp(plan->entries[i].id, id) == 0)
            return &plan->entries[i];
    return NULL;
}
static int SameEntry(const UmiCashPlanEntry *a, const UmiCashPlanEntry *b)
{
    /* Compare fields, not padding bytes or storage positions. Unit compatibility
     * was checked for the complete owners before entry differences are counted. */
    return strcmp(a->label, b->label) == 0 &&
           umi_financial_date_compare(a->date, b->date) == 0 &&
           a->amount.minor_units == b->amount.minor_units &&
           a->direction == b->direction && a->enabled == b->enabled;
}
UmiStatus UmiCashPlanCompare(const UmiCashPlan *baseline, const UmiCashPlan *scenario,
                           UmiCashPlanComparison *out)
{
    if (baseline == NULL || scenario == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiCashPlanConfig *a = &baseline->config, *b = &scenario->config;
    if (strcmp(a->opening.currency.code, b->opening.currency.code) != 0 ||
        a->opening.scale != b->opening.scale ||
        umi_financial_date_compare(a->start, b->start) != 0 ||
        umi_financial_date_compare(a->end, b->end) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A late overflow must not publish earlier differences. Keep calculation
     * private and reuse the projection owner so all hosts share date semantics. */
    UmiCashPlanComparison *report = calloc(1U, sizeof *report);
    if (report == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiCashPlanProject(baseline, &report->baseline);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanProject(scenario, &report->scenario);
    int64_t baseHeadroom = 0, scenarioHeadroom = 0;
    if (status == UMI_STATUS_OK)
        status = Difference(a->opening, report->baseline.minimum_minor, a->buffer_minor, &baseHeadroom);
    if (status == UMI_STATUS_OK)
        status = Difference(b->opening, report->scenario.minimum_minor, b->buffer_minor, &scenarioHeadroom);
    struct Delta { int64_t scenario, baseline; int64_t *out; } deltas[] = {
        {b->opening.minor_units, a->opening.minor_units, &report->opening_delta_minor},
        {b->buffer_minor, a->buffer_minor, &report->buffer_delta_minor},
        {report->scenario.total_inflow_minor, report->baseline.total_inflow_minor, &report->inflow_delta_minor},
        {report->scenario.total_outflow_minor, report->baseline.total_outflow_minor, &report->outflow_delta_minor},
        {report->scenario.closing_minor, report->baseline.closing_minor, &report->closing_delta_minor},
        {report->scenario.minimum_minor, report->baseline.minimum_minor, &report->minimum_delta_minor},
        {scenarioHeadroom, baseHeadroom, &report->minimum_headroom_delta_minor}
    };
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof deltas / sizeof deltas[0]; ++i)
        status = Difference(a->opening, deltas[i].scenario, deltas[i].baseline, deltas[i].out);
    if (status == UMI_STATUS_OK)
    {
        for (size_t i = 0U; i < baseline->count; ++i)
        {
            const UmiCashPlanEntry *entry = Find(scenario, baseline->entries[i].id);
            if (entry == NULL)
                ++report->removed;
            else if (SameEntry(&baseline->entries[i], entry))
                ++report->unchanged;
            else
                ++report->changed;
        }
        for (size_t i = 0U; i < scenario->count; ++i)
            if (Find(baseline, scenario->entries[i].id) == NULL)
                ++report->added;
        *out = *report;
    }
    free(report);
    return status;
}
