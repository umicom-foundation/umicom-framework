/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_fee_calculation.c
 *
 * PURPOSE:
 *   Exercise calculate maker or taker exchange fees plus regulatory charges.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_calculation.h"
int main(void) {

    UmiTradingFeeSchedule s;
    int64_t fee=0;
    if(umi_trading_fee_schedule_init(&s,1,2,1)!=UMI_STATUS_OK)return 1;
    if(umi_trading_fee_calculation_compute(&s,10,false,&fee)!=UMI_STATUS_OK)return 2;
    return fee==30?0:3;
}
