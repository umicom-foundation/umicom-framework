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

int main(void) { UmiPrudentialManagementActionPlan p={0}; UmiPrudentialManagementAction a; if(umi_pru_management_action_init(&a,"A",10.0,5.0,1.0)!=UMI_STATUS_OK)return 1; if(umi_pru_management_action_plan_add(&p,&a)!=UMI_STATUS_OK)return 2; return umi_pru_management_action_plan_capital_benefit(&p)==10.0?0:3; }
