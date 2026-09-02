/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_bond_schedule.c
 *
 * PURPOSE:
 *   Implement the test bond schedule behavior for
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

#include "umicom/finance/quant/bond_schedule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantBondSchedule value;
    CHECK(umi_quant_bond_schedule_init(&value, 365, 90) == UMI_STATUS_OK);
    CHECK(umi_quant_bond_schedule_period_count(&value) == 5.0);
    return 0;
}
