/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_scenario.c
 *
 * PURPOSE:
 *   Verify represent capital scenario adjustments to earnings, losses and RWA.
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

#include "umicom/finance/prudential/capital_scenario.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalScenario s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_scenario_init(&s,"ADVERSE",-5.0,10.0,1.2)!=UMI_STATUS_OK)return 1; return s.rwa_multiplier==1.2?0:2; }
