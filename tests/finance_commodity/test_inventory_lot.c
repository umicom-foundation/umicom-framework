/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_inventory_lot.c
 *
 * PURPOSE:
 *   Implement the test inventory lot behavior for
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

#include "umicom/finance/commodity/inventory_lot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityInventoryLot value;
    CHECK(umi_commodity_inventory_lot_init(&value, "LOT-1", "CMD-WTI", "FAC-1", 5000, 0, "BBL", 1000) == UMI_STATUS_OK);
    CHECK(umi_commodity_inventory_lot_valid(&value));
    return 0;
}
