/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_plan.c
 *
 * PURPOSE:
 *   Verify maintain a bounded sequence of planned capital actions by period.
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

#include "umicom/finance/prudential/capital_plan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalPlan p={0}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_plan_add(&p,"A",1,10.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_plan_add(&p,"B",2,20.0)!=UMI_STATUS_OK)return 2; return umi_pru_capital_plan_total(&p)==30.0?0:3; }
