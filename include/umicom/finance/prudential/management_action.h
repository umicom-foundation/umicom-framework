/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/management_action.h
 *
 * PURPOSE:
 *   Represent one management action with capital/liquidity benefit and execution cost.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MANAGEMENT_ACTION_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_MANAGEMENT_ACTION_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential management action data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialManagementAction { char action_id[UMI_PRU_ID_CAPACITY]; double capital_benefit; double liquidity_benefit; double execution_cost; int approved; } UmiPrudentialManagementAction;
/* Initialise a quantified management action used in stress recovery planning. */
UmiStatus umi_pru_management_action_init(UmiPrudentialManagementAction *action, const char *action_id, double capital_benefit, double liquidity_benefit, double execution_cost);

#ifdef __cplusplus
}
#endif

#endif
