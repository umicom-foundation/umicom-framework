/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_management_action.c
 *
 * PURPOSE:
 *   Verify represent one management action with capital/liquidity benefit and execution cost.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/management_action.h"

int main(void) { UmiPrudentialManagementAction a; if(umi_pru_management_action_init(&a,"DIV-CUT",10.0,5.0,1.0)!=UMI_STATUS_OK)return 1; return a.capital_benefit==10.0&&a.approved==0?0:2; }
