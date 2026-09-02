/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_historical_scenario.c
 *
 * PURPOSE:
 *   Implement the test historical scenario behavior for
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

#include "umicom/finance/enterprise/historical_scenario.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseHistoricalScenario s;CHECK(umi_enterprise_historical_scenario_init(&s,"2008")==UMI_STATUS_OK);CHECK(umi_enterprise_historical_scenario_add_shock(&s,"SPX",-0.1)==UMI_STATUS_OK);CHECK(s.count==1U);return 0; }
