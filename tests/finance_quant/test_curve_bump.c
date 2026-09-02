/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_curve_bump.c
 *
 * PURPOSE:
 *   Implement the test curve bump behavior for
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
#include "umicom/finance/quant/curve_bump.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantCurvePoint nodes[2] = {{30,0.02},{365,0.03}};
    CHECK(umi_quant_curve_bump_parallel(nodes, 2U, 0.001) == UMI_STATUS_OK);
    CHECK(nodes[1].value > 0.0309 && nodes[1].value < 0.0311);
    CHECK(umi_quant_curve_bump_node(nodes, 2U, 0U, -0.001) == UMI_STATUS_OK);
    CHECK(nodes[0].value > 0.0199 && nodes[0].value < 0.0201);
    return 0;
}
