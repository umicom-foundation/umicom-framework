/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_plan.h
 *
 * PURPOSE:
 *   Maintain a bounded sequence of planned capital actions by period.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_PLAN_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_PLAN_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential capital plan item data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialCapitalPlanItem { char action_id[UMI_PRU_ID_CAPACITY]; int32_t period; double amount; } UmiPrudentialCapitalPlanItem;
/**
 * Represent the prudential capital plan data shared with callers of this public contract.
 */
typedef struct UmiPrudentialCapitalPlan { UmiPrudentialCapitalPlanItem items[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialCapitalPlan;
/* Add a period-tagged action to a bounded prudential plan. */
UmiStatus umi_pru_capital_plan_add(UmiPrudentialCapitalPlan *plan, const char *action_id, int32_t period, double amount);
/* Sum planned positive capacity across every action. */
double umi_pru_capital_plan_total(const UmiPrudentialCapitalPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
