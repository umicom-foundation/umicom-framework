/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/management_action_plan.h
 *
 * PURPOSE:
 *   Aggregate management actions available to a stress-response plan.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MANAGEMENT_ACTION_PLAN_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MANAGEMENT_ACTION_PLAN_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/management_action.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential management action plan data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialManagementActionPlan { UmiPrudentialManagementAction actions[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialManagementActionPlan;
/* Add one stress-response management action to the bounded plan. */
UmiStatus umi_pru_management_action_plan_add(UmiPrudentialManagementActionPlan *plan, const UmiPrudentialManagementAction *action);
/* Sum quantified capital benefits from all approved or proposed actions. */
double umi_pru_management_action_plan_capital_benefit(const UmiPrudentialManagementActionPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
