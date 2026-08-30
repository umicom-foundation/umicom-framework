/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_unit_catalogue.c
 *
 * PURPOSE:
 *   Implement the test unit catalogue behavior for
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

#include "umicom/finance/commodity/unit_catalogue.h"

int main(void)
{
    UmiCommodityUnitCatalogue catalogue;
    UmiCommodityUnitOfMeasure item;
    umi_commodity_unit_catalogue_init(&catalogue);
    CHECK(umi_commodity_unit_of_measure_init(&item, "MT", "MASS", 1000, 1) == UMI_STATUS_OK);
    CHECK(umi_commodity_unit_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_unit_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_unit_catalogue_find(&catalogue, "MT") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
