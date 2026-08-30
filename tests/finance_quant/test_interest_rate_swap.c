/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_interest_rate_swap.c
 *
 * PURPOSE:
 *   Implement the test interest rate swap behavior for
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

#include "umicom/finance/quant/interest_rate_swap.h"

int main(void)
{
    UmiQuantInterestRateSwap value;
    CHECK(umi_quant_interest_rate_swap_init(&value, 1000000.0, 0.04, 0.045, 0.5) == UMI_STATUS_OK);
    CHECK(umi_quant_interest_rate_swap_period_value(&value) > 2499.0 && umi_quant_interest_rate_swap_period_value(&value) < 2501.0);
    return 0;
}
