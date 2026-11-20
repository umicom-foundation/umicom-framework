/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/portfolio.c
 *
 * PURPOSE:
 *   Group checked cash projections over one date range by their exact units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/cash_planning/portfolio.h"
#include "umicom/finance/money.h"
#include <stdlib.h>
#include <string.h>

static int CompareUnit(const UmiMoney *left, const UmiMoney *right)
{
    /* Keep alternate scales as separate groups, including for the same
     * currency. Sorting the observed units does not change their amounts. */
    int currency = strcmp(left->currency.code, right->currency.code);
    return currency != 0 ? currency : (int)left->scale - (int)right->scale;
}

static UmiStatus AddForecast(UmiCashPlanPortfolio *portfolio, const UmiCashPlanForecast *forecast)
{
    /* Insert the exact unit in the private sorted table. Each admitted input
     * can create at most one group inside the portfolio capacity. */
    UmiMoney unit = forecast->config.opening;
    unit.minor_units = 0;
    size_t index = 0U;
    while (index < portfolio->group_count && CompareUnit(&portfolio->groups[index].opening, &unit) < 0)
        ++index;
    if (index == portfolio->group_count || CompareUnit(&portfolio->groups[index].opening, &unit) != 0) {
        size_t remaining = portfolio->group_count - index;
        if (remaining != 0U)
            memmove(&portfolio->groups[index + 1U], &portfolio->groups[index], remaining * sizeof portfolio->groups[0]);
        portfolio->groups[index] = (UmiCashPlanPortfolioRow){.opening = unit, .inflow = unit, .outflow = unit, .closing = unit};
        ++portfolio->group_count;
    }

    /* Use the existing exact money arithmetic for every subtotal. The
     * off-stack forecast remains the authority for dated plan semantics. */
    UmiCashPlanPortfolioRow *group = &portfolio->groups[index];
    UmiMoney *totals[] = {&group->opening, &group->inflow, &group->outflow, &group->closing};
    const int64_t amounts[] = {forecast->config.opening.minor_units, forecast->total_inflow_minor,
        forecast->total_outflow_minor, forecast->closing_minor};
    for (size_t field = 0U; field < sizeof totals / sizeof totals[0]; ++field) {
        unit.minor_units = amounts[field];
        UmiStatus status = umi_money_add(totals[field], &unit, totals[field]);
        if (status != UMI_STATUS_OK) return status;
    }
    ++group->plan_count;
    if (forecast->has_shortfall) ++group->plans_with_shortfalls;
    return UMI_STATUS_OK;
}

UmiStatus UmiCashPlanProjectPortfolio(const UmiCashPlan *const *plans, size_t count,
    UmiCashPlanPortfolio *out)
{
    if (plans == NULL || out == NULL || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CASH_PLAN_PORTFOLIO_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Admit identities and the common date range before projecting. A plan
     * cannot contribute twice through two references to the same owner. */
    UmiCashPlanPortfolio candidate = {.plan_count = count};
    for (size_t index = 0U; index < count; ++index) {
        UmiCashPlanConfig config;
        UmiStatus status = UmiCashPlanRead(plans[index], &config);
        if (status != UMI_STATUS_OK) return status;
        for (size_t previous = 0U; previous < index; ++previous)
            if (plans[index] == plans[previous]) return UMI_STATUS_INVALID_ARGUMENT;
        if (index == 0U) {
            candidate.start = config.start;
            candidate.end = config.end;
        } else if (umi_financial_date_compare(candidate.start, config.start) != 0 ||
                   umi_financial_date_compare(candidate.end, config.end) != 0) return UMI_STATUS_INVALID_ARGUMENT;
        candidate.revisions[index] = UmiCashPlanRevision(plans[index]);
    }

    /* Reuse one projection result off the stack. Publish only after every
     * selected plan and group has succeeded; leave the old result on failure. */
    UmiCashPlanForecast *forecast = malloc(sizeof *forecast);
    if (forecast == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        status = UmiCashPlanProject(plans[index], forecast);
        if (status == UMI_STATUS_OK) status = AddForecast(&candidate, forecast);
    }
    free(forecast);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
