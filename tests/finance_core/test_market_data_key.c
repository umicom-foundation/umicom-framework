/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_market_data_key.c
 *
 * PURPOSE:
 *   Exercise the market data key financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/market_data_key.h"

int main(void)
{
    UmiMarketDataKey x; CHECK(umi_market_data_key_init(&x,"ID","Name","CODE",1U)==UMI_STATUS_OK); CHECK(umi_market_data_key_is_valid(&x));
    return 0;
}
