/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_curve_node.c
 *
 * PURPOSE:
 *   Implement the test curve node behavior for
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

#include "umicom/finance/quant/curve_node.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantCurveNode value;
    CHECK(umi_quant_curve_node_init(&value, 365, 0.04) == UMI_STATUS_OK);
    CHECK(umi_quant_curve_node_quoted_value(&value) > 0.039 && umi_quant_curve_node_quoted_value(&value) < 0.041);
    return 0;
}
