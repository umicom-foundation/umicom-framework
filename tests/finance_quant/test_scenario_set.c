/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_scenario_set.c
 *
 * PURPOSE:
 *   Implement the test scenario set behavior for
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

#include <math.h>
#include "umicom/finance/quant/scenario_set.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantScenarioSet set; umi_quant_scenario_set_init(&set); CHECK(umi_quant_scenario_set_add(&set,0.01,-0.05,0.02,0.4)==UMI_STATUS_OK); CHECK(umi_quant_scenario_set_add(&set,-0.01,0.05,-0.02,0.6)==UMI_STATUS_OK); CHECK(umi_quant_scenario_set_total_weight(&set)>0.99);
    return 0;
}
