/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_management_action_plan.c
 *
 * PURPOSE:
 *   Verify aggregate management actions available to a stress-response plan.
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
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialManagementActionPlan p={0}; UmiPrudentialManagementAction a; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_management_action_init(&a,"A",10.0,5.0,1.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_management_action_plan_add(&p,&a)!=UMI_STATUS_OK)return 2; return umi_pru_management_action_plan_capital_benefit(&p)==10.0?0:3; }
