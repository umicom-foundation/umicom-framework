/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_trade_registry.c
 *
 * PURPOSE:
 *   Exercise the trade registry financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/trade_registry.h"

int main(void)
{
    UmiTradeRegistry r; UmiTradeDescriptor x; umi_trade_registry_init(&r); CHECK(umi_trade_descriptor_init(&x,"T","Trade","BOOK",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_trade_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
