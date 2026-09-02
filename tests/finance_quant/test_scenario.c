/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_scenario.c
 *
 * PURPOSE:
 *   Implement the test scenario behavior for
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

#include "umicom/finance/quant/scenario.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantScenario value;
    CHECK(umi_quant_scenario_init(&value, 0.01, -0.05, 0.02, 1.0) == UMI_STATUS_OK);
    CHECK(umi_quant_scenario_magnitude(&value) > 0.079 && umi_quant_scenario_magnitude(&value) < 0.081);
    return 0;
}
