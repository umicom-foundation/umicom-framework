/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_storage_facility.c
 *
 * PURPOSE:
 *   Implement the test storage facility behavior for
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

#include "umicom/finance/commodity/storage_facility.h"

int main(void)
{
    UmiCommodityStorageFacility value;
    CHECK(umi_commodity_storage_facility_init(&value, "FAC-1", "LOC-1", 100000, 0, "BBL") == UMI_STATUS_OK);
    CHECK(umi_commodity_storage_facility_valid(&value));
    CHECK(value.capacity.units == 100000);
    return 0;
}
