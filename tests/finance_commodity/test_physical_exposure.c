/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_physical_exposure.c
 *
 * PURPOSE:
 *   Implement the test physical exposure behavior for
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

#include "umicom/finance/commodity/physical_exposure.h"

int main(void)
{
    UmiCommodityPhysicalExposure exposure;
    CHECK(umi_commodity_physical_exposure_init(&exposure, "CMD-WTI", 0, "BBL") == UMI_STATUS_OK);
    CHECK(umi_commodity_physical_exposure_add(&exposure, 1000, 300) == UMI_STATUS_OK);
    CHECK(umi_commodity_physical_exposure_net(&exposure) == 700);
    return 0;
}
