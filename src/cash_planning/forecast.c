/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cash_planning/forecast.c
 * PURPOSE: Group dated assumptions and publish a complete overflow-checked projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "plan_private.h"
#include "umicom/finance/money.h"
#include <stdlib.h>
/* Reuse canonical checked money operations; every intermediate retains the
 * plan's currency and scale. Never use floating point for cash amounts. */
static UmiStatus Arithmetic(UmiMoney unit, int64_t a, int64_t b, int subtract, int64_t *out)
{
    UmiMoney left = unit, right = unit, result;
    left.minor_units = a;
    right.minor_units = b;
    UmiStatus status =
        subtract ? umi_money_subtract(&left, &right, &result) : umi_money_add(&left, &right, &result);
    if (status == UMI_STATUS_OK)
        *out = result.minor_units;
    return status;
}
UmiStatus UmiCashPlanProject(const UmiCashPlan *plan, UmiCashPlanForecast *out)
{
    if (plan == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Allocate the bounded report off the stack for mobile and native hosts.
     * A refused late row cannot leak a partly calculated caller-visible report. */
    UmiCashPlanForecast *forecast = calloc(1U, sizeof *forecast);
    if (forecast == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    forecast->config = plan->config;
    forecast->closing_minor = forecast->minimum_minor = plan->config.opening.minor_units;
    forecast->minimum_date = plan->config.start;
    int64_t openingHeadroom = 0;
    UmiStatus status = Arithmetic(plan->config.opening, forecast->closing_minor, plan->config.buffer_minor, 1,
                                  &openingHeadroom);
    if (status == UMI_STATUS_OK && openingHeadroom < 0)
    {
        forecast->has_shortfall = 1;
        forecast->first_shortfall_date = plan->config.start;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < plan->count; ++i)
    {
        const UmiCashPlanEntry *entry = &plan->entries[i];
        if (!entry->enabled)
        {
            ++forecast->disabled;
            continue;
        }
        if (umi_financial_date_compare(entry->date, plan->config.start) < 0)
        {
            ++forecast->before_start;
            continue;
        }
        if (umi_financial_date_compare(entry->date, plan->config.end) > 0)
        {
            ++forecast->after_end;
            continue;
        }
        size_t at = 0U;
        while (at < forecast->day_count &&
               umi_financial_date_compare(forecast->days[at].date, entry->date) < 0)
            ++at;
        if (at == forecast->day_count ||
            umi_financial_date_compare(forecast->days[at].date, entry->date) != 0)
        {
            for (size_t j = forecast->day_count; j > at; --j)
                forecast->days[j] = forecast->days[j - 1U];
            forecast->days[at] = (UmiCashPlanDay){.date = entry->date};
            ++forecast->day_count;
        }
        UmiCashPlanDay *day = &forecast->days[at];
        int64_t *total =
            entry->direction == UMI_FINANCIAL_DIRECTION_PAY ? &day->outflow_minor : &day->inflow_minor;
        status = Arithmetic(plan->config.opening, *total, entry->amount.minor_units, 0, total);
        ++day->entries;
        ++forecast->included;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < forecast->day_count; ++i)
    {
        UmiCashPlanDay *day = &forecast->days[i];
        int64_t net = 0;
        status = Arithmetic(plan->config.opening, day->inflow_minor, day->outflow_minor, 1, &net);
        if (status == UMI_STATUS_OK)
            status = Arithmetic(plan->config.opening, forecast->closing_minor, net, 0, &day->closing_minor);
        if (status == UMI_STATUS_OK)
            status = Arithmetic(plan->config.opening, day->closing_minor, plan->config.buffer_minor, 1,
                                &day->headroom_minor);
        if (status == UMI_STATUS_OK)
            status = Arithmetic(plan->config.opening, forecast->total_inflow_minor, day->inflow_minor, 0,
                                &forecast->total_inflow_minor);
        if (status == UMI_STATUS_OK)
            status = Arithmetic(plan->config.opening, forecast->total_outflow_minor, day->outflow_minor, 0,
                                &forecast->total_outflow_minor);
        forecast->closing_minor = day->closing_minor;
        if (day->closing_minor < forecast->minimum_minor)
        {
            forecast->minimum_minor = day->closing_minor;
            forecast->minimum_date = day->date;
        }
        if (day->headroom_minor < 0 && !forecast->has_shortfall)
        {
            forecast->has_shortfall = 1;
            forecast->first_shortfall_date = day->date;
        }
    }
    if (status == UMI_STATUS_OK)
        *out = *forecast;
    free(forecast);
    return status;
}
