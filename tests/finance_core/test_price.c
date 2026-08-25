/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_price.c
 *
 * PURPOSE:
 *   Exercise the price financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/price.h"

int main(void)
{
    UmiFinancialPrice p; CHECK(umi_price_init(&p,101.25,2U)==UMI_STATUS_OK);
    return 0;
}
