/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cash_planning/plan.h
 * PURPOSE: Own dated cash assumptions and calculate checked end-of-day balances.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CASH_PLANNING_PLAN_H
#define UMICOM_CASH_PLANNING_PLAN_H
#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CASH_PLAN_CAPACITY 128U
    typedef struct UmiCashPlan UmiCashPlan;
    typedef struct UmiCashPlanConfig
    {
        char title[128];
        UmiFinancialDate start, end;
        UmiMoney opening;
        int64_t buffer_minor;
    } UmiCashPlanConfig;
    typedef struct UmiCashPlanEntry
    {
        char id[48], label[128];
        UmiFinancialDate date;
        UmiMoney amount;
        UmiFinancialDirection direction;
        int enabled;
    } UmiCashPlanEntry;
    typedef struct UmiCashPlanDay
    {
        UmiFinancialDate date;
        int64_t inflow_minor, outflow_minor, closing_minor, headroom_minor;
        size_t entries;
    } UmiCashPlanDay;
    typedef struct UmiCashPlanForecast
    {
        UmiCashPlanConfig config;
        UmiCashPlanDay days[UMI_CASH_PLAN_CAPACITY];
        size_t day_count, included, disabled, before_start, after_end;
        int64_t total_inflow_minor, total_outflow_minor, closing_minor, minimum_minor;
        UmiFinancialDate minimum_date, first_shortfall_date;
        int has_shortfall;
    } UmiCashPlanForecast;
    /* One owner thread. Money uses an explicit scale 0..9 and uppercase currency;
 * currencies and scales are never converted implicitly. Opening may be signed;
 * buffer and entry amounts must be nonnegative. Titles/IDs/labels are required
 * bounded UTF-8 without ASCII controls. Date range is inclusive, 1600..9999.
 * Create/Copy clear *out on failure. Destroy accepts NULL. */
    UmiStatus UmiCashPlanCreate(const UmiCashPlanConfig *config, UmiCashPlan **out);
    void UmiCashPlanDestroy(UmiCashPlan *plan);
    UmiStatus UmiCashPlanCopy(const UmiCashPlan *plan, UmiCashPlan **out);
    UmiStatus UmiCashPlanRead(const UmiCashPlan *plan, UmiCashPlanConfig *out);
    size_t UmiCashPlanCount(const UmiCashPlan *plan);
    uint64_t UmiCashPlanRevision(const UmiCashPlan *plan);
    /* Mutations validate completely before publishing. Failure leaves content and
 * revision unchanged. SetConfig refuses a currency/scale change incompatible
 * with any entry, including disabled entries. Dates outside the range remain
 * stored and are reported separately. IDs must be unique, including on Replace.
 * At/Read publish copies only on success; never borrow internal array storage. */
    UmiStatus UmiCashPlanSetConfig(UmiCashPlan *plan, const UmiCashPlanConfig *config);
    UmiStatus UmiCashPlanAdd(UmiCashPlan *plan, const UmiCashPlanEntry *entry);
    UmiStatus UmiCashPlanReplace(UmiCashPlan *plan, size_t index, const UmiCashPlanEntry *entry);
    UmiStatus UmiCashPlanRemove(UmiCashPlan *plan, size_t index);
    UmiStatus UmiCashPlanAt(const UmiCashPlan *plan, size_t index, UmiCashPlanEntry *out);
    /* Group enabled, in-range assumptions by calendar date. Each row is an end-
 * of-day projection, not an intraday liquidity guarantee. Opening is the start
 * of the first day; minimum/shortfall include opening. Equal minima retain the
 * earliest date. Disabled entries are counted first, outside dates separately.
 * Checked integer arithmetic rejects any overflowing intermediate/totals even
 * when later offsets could fit. Failure leaves *out unchanged. No account,
 * payment, order, FX conversion or bank holiday adjustment is performed. */
    UmiStatus UmiCashPlanProject(const UmiCashPlan *plan, UmiCashPlanForecast *out);
    /* Strict YYYY-MM-DD conversion; no locale, timezone or permissive date rollover.
 * Parse accepts exactly ten bytes. Format needs eleven bytes including NUL.
 * Outputs remain unchanged on failure. */
    UmiStatus UmiCashPlanDateParse(const char *text, size_t bytes, UmiFinancialDate *out);
    UmiStatus UmiCashPlanDateFormat(UmiFinancialDate date, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
