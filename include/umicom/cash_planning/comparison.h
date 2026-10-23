/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cash_planning/comparison.h
 * PURPOSE: Compare stored cash scenarios using the same projection and money owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CASH_PLANNING_COMPARISON_H
#define UMICOM_CASH_PLANNING_COMPARISON_H
#include "umicom/cash_planning/plan.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A comparison owns copies, never pointers into either source plan. Allocate
 * this bounded report off the stack in hosts with small thread stacks. */
typedef struct UmiCashPlanComparison
{
    UmiCashPlanForecast baseline, scenario;
    int64_t opening_delta_minor, buffer_delta_minor;
    int64_t inflow_delta_minor, outflow_delta_minor;
    int64_t closing_delta_minor, minimum_delta_minor, minimum_headroom_delta_minor;
    size_t added, removed, changed, unchanged;
} UmiCashPlanComparison;

/* Compare on the plans' owner thread. Currency, scale and inclusive date range
 * must match; otherwise return INVALID_ARGUMENT without changing *out. No FX,
 * rescaling, date alignment, mutation, payment or trade is performed.
 * Every delta is scenario minus baseline in the explicit common minor unit.
 * Minimum balances may occur on different dates; inspect the two forecasts.
 * Minimum headroom uses each plan's own buffer. Entry counts match by stable ID
 * regardless of storage order. Changed includes label, date, amount, direction
 * or enabled-state edits, including disabled and out-of-range assumptions.
 * Renaming an ID is one removal and one addition. Title-only changes affect no
 * entry count. All projections and subtractions are checked; any failure leaves
 * the entire report unchanged, even if both individual projections would fit. */
UmiStatus UmiCashPlanCompare(const UmiCashPlan *baseline, const UmiCashPlan *scenario,
                           UmiCashPlanComparison *out);
#ifdef __cplusplus
}
#endif
#endif
