/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_margin_profile.c
 *
 * PURPOSE:
 *   Exercise define conservative initial and maintenance margin ratios in basis points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/margin_profile.h"
int main(void) {
    UmiTradingMarginProfile v;
     if(umi_trading_margin_profile_init(&v,5000U,3000U,500U)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_margin_profile_valid(&v)) return 2;
     return 0;
}
