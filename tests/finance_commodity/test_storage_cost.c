/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_storage_cost.c
 *
 * PURPOSE:
 *   Implement the test storage cost behavior for
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

#include "umicom/finance/commodity/storage_cost.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityStorageCost cost;
    UmiCurrency currency = {{'U','S','D','\0'}};
    int64_t amount = 0;
    CHECK(umi_commodity_storage_cost_init(&cost, "FAC-1", 2, &currency) == UMI_STATUS_OK);
    CHECK(umi_commodity_storage_cost_calculate(&cost, 100, 3, &amount) == UMI_STATUS_OK);
    CHECK(amount == 600);
    return 0;
}
