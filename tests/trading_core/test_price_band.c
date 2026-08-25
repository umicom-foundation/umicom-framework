/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_price_band.c
 *
 * PURPOSE:
 *   Exercise define price protection bands around a reference price without overflow-prone multiplication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/price_band.h"
int main(void) {

    UmiTradingPriceBand b;
     if(umi_trading_price_band_init(&b,100000,100U,200U)!=UMI_STATUS_OK)return 1;
    if(!umi_trading_price_band_contains(&b,101000)||!umi_trading_price_band_contains(&b,99000))return 2;
    if(umi_trading_price_band_contains(&b,98000))return 3;
     return 0;
}
