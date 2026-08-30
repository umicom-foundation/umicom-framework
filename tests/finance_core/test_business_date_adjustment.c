/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_business_date_adjustment.c
 *
 * PURPOSE:
 *   Exercise the business date adjustment financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/business_date_adjustment.h"

int main(void)
{
    UmiBusinessDateAdjustment p; umi_business_date_adjustment_default(&p); CHECK(umi_business_date_adjustment_allows(&p,10U,1U));
    return 0;
}
