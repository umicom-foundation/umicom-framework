/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
#include <string.h>
#include "umicom/finance/quant/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[16];
    CHECK(umi_quant_copy_text(text, sizeof text, "USD-OIS") == UMI_STATUS_OK);
    CHECK(strcmp(text, "USD-OIS") == 0);
    CHECK(umi_quant_number_valid(1.25));
    CHECK(umi_quant_normal_cdf(0.0) > 0.499 && umi_quant_normal_cdf(0.0) < 0.501);
    return 0;
}
