/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_credit_spread.c
 *
 * PURPOSE:
 *   Implement the test credit spread behavior for
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

#include "umicom/finance/quant/credit_spread.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantCreditSpread value;
    CHECK(umi_quant_credit_spread_init(&value, 0.055, 0.04) == UMI_STATUS_OK);
    CHECK(umi_quant_credit_spread_spread(&value) > 0.0149 && umi_quant_credit_spread_spread(&value) < 0.0151);
    return 0;
}
