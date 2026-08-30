/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_pricing_result.c
 *
 * PURPOSE:
 *   Exercise the pricing result financial-core contract.
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
#include "umicom/finance/core/pricing_result.h"

int main(void)
{
    UmiPricingResult x; UmiMoney m={10,2U,{{'U','S','D','\0'}}}; CHECK(umi_pricing_result_init(&x,"ID",m,(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_pricing_result_is_valid(&x));
    return 0;
}
