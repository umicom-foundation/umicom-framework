/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_market_data_snapshot.c
 *
 * PURPOSE:
 *   Exercise the market data snapshot financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/market_data_snapshot.h"

int main(void)
{
    UmiMarketDataSnapshot s; CHECK(umi_market_data_snapshot_init(&s,"S")==UMI_STATUS_OK); CHECK(umi_market_data_snapshot_is_valid(&s));
    return 0;
}
