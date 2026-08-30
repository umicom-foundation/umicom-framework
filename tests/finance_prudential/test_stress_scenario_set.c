/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_scenario_set.c
 *
 * PURPOSE:
 *   Verify maintain a bounded enterprise stress-scenario catalogue.
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

#include "umicom/finance/prudential/stress_scenario_set.h"

int main(void) { UmiPrudentialStressScenarioSet set={0}; UmiPrudentialStressScenario s; if(umi_pru_stress_scenario_init(&s,"S","S",UMI_PRU_SEVERITY_WARNING,30U)!=UMI_STATUS_OK)return 1; if(umi_pru_stress_scenario_set_add(&set,&s)!=UMI_STATUS_OK)return 2; if(umi_pru_stress_scenario_set_add(&set,&s)!=UMI_STATUS_ALREADY_EXISTS)return 3; return 0; }
