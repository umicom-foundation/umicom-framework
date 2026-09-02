/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/management_action.c
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

#include "umicom/finance/prudential/management_action.h"

#include <string.h>
/*
 * Initialise pru management action from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_pru_management_action_init(UmiPrudentialManagementAction *action,const char *action_id,double capital_benefit,double liquidity_benefit,double execution_cost) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(action==NULL||action_id==NULL||capital_benefit<0.0||liquidity_benefit<0.0||execution_cost<0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(action,0,sizeof *action); s=umi_pru_copy_text(action->action_id,sizeof action->action_id,action_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; action->capital_benefit=capital_benefit; action->liquidity_benefit=liquidity_benefit; action->execution_cost=execution_cost; action->approved=0; return UMI_STATUS_OK; }
