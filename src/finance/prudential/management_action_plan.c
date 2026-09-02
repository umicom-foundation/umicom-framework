/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/management_action_plan.c
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

#include "umicom/finance/prudential/management_action_plan.h"

/*
 * Add pru management action plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_pru_management_action_plan_add(UmiPrudentialManagementActionPlan *plan,const UmiPrudentialManagementAction *action) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL||action==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; plan->actions[plan->count++]=*action; return UMI_STATUS_OK; }
/*
 * Provide the pru management action plan capital benefit operation used by this module and
 * its client applications.
 */
double umi_pru_management_action_plan_capital_benefit(const UmiPrudentialManagementActionPlan *plan) { size_t i; double total=0.0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(plan==NULL)return 0.0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<plan->count;++i)total+=plan->actions[i].capital_benefit; return total; }
