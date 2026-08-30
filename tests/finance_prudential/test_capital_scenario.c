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

int main(void) { UmiPrudentialCapitalScenario s; if(umi_pru_capital_scenario_init(&s,"ADVERSE",-5.0,10.0,1.2)!=UMI_STATUS_OK)return 1; return s.rwa_multiplier==1.2?0:2; }
