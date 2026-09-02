/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_commodity_catalogue.c
 *
 * PURPOSE:
 *   Implement the test commodity catalogue behavior for
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

#include "umicom/finance/commodity/commodity_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityCatalogue catalogue;
    UmiCommodityDescriptor item;
    umi_commodity_commodity_catalogue_init(&catalogue);
    CHECK(umi_commodity_commodity_descriptor_init(&item, "CMD-WTI", "West Texas Intermediate", "WTI", UMI_COMMODITY_KIND_ENERGY, &(UmiCurrency){{'U','S','D','\0'}}, true) == UMI_STATUS_OK);
    CHECK(umi_commodity_commodity_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_commodity_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_commodity_catalogue_find(&catalogue, "CMD-WTI") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
