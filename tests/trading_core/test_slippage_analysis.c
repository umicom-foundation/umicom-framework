/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_slippage_analysis.c
 *
 * PURPOSE:
 *   Exercise aggregate execution slippage in integer ticks with side-aware sign.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/slippage_analysis.h"
int main(void) {

    UmiTradingSlippageAnalysis a;
    umi_trading_slippage_analysis_init(&a);
    if(umi_trading_slippage_analysis_add(&a,UMI_SIDE_BUY,100,102)!=UMI_STATUS_OK)return 1;
    return umi_trading_slippage_analysis_average(&a)==2?0:2;
}
