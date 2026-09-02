/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_location_catalogue.c
 *
 * PURPOSE:
 *   Implement the test location catalogue behavior for
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

#include "umicom/finance/commodity/location_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityLocationCatalogue catalogue;
    UmiCommodityLocation item;
    umi_commodity_location_catalogue_init(&catalogue);
    CHECK(umi_commodity_location_init(&item, "LOC-1", "Terminal One", "GB") == UMI_STATUS_OK);
    CHECK(umi_commodity_location_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_location_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_location_catalogue_find(&catalogue, "LOC-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
