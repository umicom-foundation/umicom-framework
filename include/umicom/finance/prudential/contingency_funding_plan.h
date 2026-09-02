/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/contingency_funding_plan.h
 *
 * PURPOSE:
 *   Store executable contingency funding actions and available capacity.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CONTINGENCY_FUNDING_PLAN_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CONTINGENCY_FUNDING_PLAN_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential contingency funding plan item data shared with callers of this
 * public contract.
 */
typedef struct UmiPrudentialContingencyFundingPlanItem { char action_id[UMI_PRU_ID_CAPACITY]; int32_t period; double amount; } UmiPrudentialContingencyFundingPlanItem;
/**
 * Represent the prudential contingency funding plan data shared with callers of this
 * public contract.
 */
typedef struct UmiPrudentialContingencyFundingPlan { UmiPrudentialContingencyFundingPlanItem items[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialContingencyFundingPlan;
/* Add a period-tagged action to a bounded prudential plan. */
UmiStatus umi_pru_contingency_funding_plan_add(UmiPrudentialContingencyFundingPlan *plan, const char *action_id, int32_t period, double amount);
/* Sum planned positive capacity across every action. */
double umi_pru_contingency_funding_plan_total(const UmiPrudentialContingencyFundingPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
