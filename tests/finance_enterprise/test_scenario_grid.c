/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_scenario_grid.c
 *
 * PURPOSE:
 *   Implement the test scenario grid behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/scenario_grid.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseScenarioGrid g;UmiEnterpriseHistoricalScenario s;umi_enterprise_scenario_grid_init(&g);CHECK(umi_enterprise_historical_scenario_init(&s,"s")==UMI_STATUS_OK);CHECK(umi_enterprise_scenario_grid_add(&g,&s)==UMI_STATUS_OK);CHECK(g.count==1U);return 0;}
