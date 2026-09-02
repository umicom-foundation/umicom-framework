/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_monte_carlo_config.c
 *
 * PURPOSE:
 *   Implement the test monte carlo config behavior for
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

#include "umicom/finance/quant/monte_carlo_config.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantMonteCarloConfig value;
    CHECK(umi_quant_monte_carlo_config_init(&value, 1000U, 12U, 42U) == UMI_STATUS_OK);
    CHECK(umi_quant_monte_carlo_config_work_units(&value) == 12000.0);
    return 0;
}
