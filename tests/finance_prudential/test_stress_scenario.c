/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_scenario.c
 *
 * PURPOSE:
 *   Verify define one governed enterprise stress scenario and its severity.
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

#include "umicom/finance/prudential/stress_scenario.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressScenario s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_scenario_init(&s,"S1","Severe",UMI_PRU_SEVERITY_CRITICAL,90U)!=UMI_STATUS_OK)return 1; return s.horizon_days==90U?0:2; }
