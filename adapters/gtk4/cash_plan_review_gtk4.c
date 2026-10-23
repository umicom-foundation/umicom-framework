/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/cash_plan_review_gtk4.c
 * PURPOSE: Explain cash scenario differences before a loaded plan can replace its baseline.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cash_plan_private.h"
#include "umicom/cash_planning/comparison.h"

static void Metric(GString *text, const char *name, int64_t baseline, int64_t scenario,
                   int64_t delta, uint8_t scale)
{
    char a[UMI_DECIMAL_TEXT_MAX], b[UMI_DECIMAL_TEXT_MAX], change[UMI_DECIMAL_TEXT_MAX];
    (void)UmiDecimalFormat((UmiDecimal){baseline, scale}, a, sizeof a);
    (void)UmiDecimalFormat((UmiDecimal){scenario, scale}, b, sizeof b);
    (void)UmiDecimalFormat((UmiDecimal){delta, scale}, change, sizeof change);
    g_string_append_printf(text, "%s: current %s | loaded %s | change %s\n", name, a, b, change);
}
static void Shortfall(GString *text, const char *name, const UmiCashPlanForecast *forecast)
{
    char minimum[11], first[11];
    (void)UmiCashPlanDateFormat(forecast->minimum_date, minimum, sizeof minimum);
    g_string_append_printf(text, "%s minimum date: %s. ", name, minimum);
    if (forecast->has_shortfall)
    {
        (void)UmiCashPlanDateFormat(forecast->first_shortfall_date, first, sizeof first);
        g_string_append_printf(text, "First buffer shortfall: %s.\n", first);
    }
    else
        g_string_append(text, "No opening or end-of-day buffer shortfall.\n");
    g_string_append_printf(text, "%s entries: included %zu | disabled %zu | before range %zu | after range %zu\n",
        name, forecast->included, forecast->disabled, forecast->before_start, forecast->after_end);
}
char *UmiCashPanelComparisonText(const UmiCashPlan *baseline, const UmiCashPlan *scenario)
{
    GString *text = g_string_new("LOADED PLAN COMPARISON (not applied)\n");
    UmiCashPlanConfig a, b;
    UmiStatus status = UmiCashPlanRead(baseline, &a);
    if (status == UMI_STATUS_OK)
        status = UmiCashPlanRead(scenario, &b);
    if (status != UMI_STATUS_OK)
    {
        g_string_append_printf(text, "Comparison unavailable: %s.\n", umi_status_text(status));
        return g_string_free(text, FALSE);
    }
    /* Mismatches are explicit. A separately reviewed whole-plan replacement is
     * still possible, but a numerical comparison must not imply FX conversion. */
    int mismatch = 0;
    if (strcmp(a.opening.currency.code, b.opening.currency.code) != 0)
    {
        g_string_append_printf(text, "Currency differs: current %s | loaded %s.\n",
            a.opening.currency.code, b.opening.currency.code);
        mismatch = 1;
    }
    if (a.opening.scale != b.opening.scale)
    {
        g_string_append_printf(text, "Decimal places differ: current %u | loaded %u.\n",
            (unsigned)a.opening.scale, (unsigned)b.opening.scale);
        mismatch = 1;
    }
    if (umi_financial_date_compare(a.start, b.start) != 0 ||
        umi_financial_date_compare(a.end, b.end) != 0)
    {
        char as[11], ae[11], bs[11], be[11];
        (void)UmiCashPlanDateFormat(a.start, as, sizeof as);
        (void)UmiCashPlanDateFormat(a.end, ae, sizeof ae);
        (void)UmiCashPlanDateFormat(b.start, bs, sizeof bs);
        (void)UmiCashPlanDateFormat(b.end, be, sizeof be);
        g_string_append_printf(text, "Date range differs: current %s to %s | loaded %s to %s.\n", as, ae, bs, be);
        mismatch = 1;
    }
    if (mismatch)
        g_string_append(text, "No numeric differences calculated. Review both complete documents below.\n");
    else
    {
        UmiCashPlanComparison *report = g_new0(UmiCashPlanComparison, 1);
        status = UmiCashPlanCompare(baseline, scenario, report);
        if (status != UMI_STATUS_OK)
            g_string_append_printf(text, "Comparison unavailable: %s. No partial differences are shown.\n", umi_status_text(status));
        else
        {
            const UmiCashPlanForecast *current = &report->baseline, *loaded = &report->scenario;
            g_string_append_printf(text, "%s, %u decimal places. Change means loaded minus current.\n",
                a.opening.currency.code, (unsigned)a.opening.scale);
            Metric(text, "Opening", a.opening.minor_units, b.opening.minor_units, report->opening_delta_minor, a.opening.scale);
            Metric(text, "Buffer", a.buffer_minor, b.buffer_minor, report->buffer_delta_minor, a.opening.scale);
            Metric(text, "Total inflow", current->total_inflow_minor, loaded->total_inflow_minor, report->inflow_delta_minor, a.opening.scale);
            Metric(text, "Total outflow", current->total_outflow_minor, loaded->total_outflow_minor, report->outflow_delta_minor, a.opening.scale);
            Metric(text, "Closing", current->closing_minor, loaded->closing_minor, report->closing_delta_minor, a.opening.scale);
            Metric(text, "Minimum", current->minimum_minor, loaded->minimum_minor, report->minimum_delta_minor, a.opening.scale);
            char headroom[UMI_DECIMAL_TEXT_MAX];
            (void)UmiDecimalFormat((UmiDecimal){report->minimum_headroom_delta_minor, a.opening.scale}, headroom, sizeof headroom);
            g_string_append_printf(text, "Minimum headroom change (each plan's own buffer): %s\n", headroom);
            Shortfall(text, "Current", current);
            Shortfall(text, "Loaded", loaded);
            g_string_append_printf(text, "Entry IDs: added %zu | removed %zu | changed %zu | unchanged %zu\n",
                report->added, report->removed, report->changed, report->unchanged);
            g_string_append(text, "Counts include disabled and out-of-range entries. Minimum dates may differ.\n");
        }
        g_free(report);
    }
    g_string_append(text, "These are stored calendar-day assumptions, not intraday liquidity or live account balances.\n");
    return g_string_free(text, FALSE);
}
