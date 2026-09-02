/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_loss_allowance.c
 *
 * PURPOSE:
 *   Implement the test loss allowance behavior for
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

#include "umicom/finance/commodity/loss_allowance.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityLossAllowance value;
    CHECK(umi_commodity_loss_allowance_init(&value, "CTR-1", 25) == UMI_STATUS_OK);
    CHECK(umi_commodity_loss_allowance_valid(&value));
    return 0;
}
