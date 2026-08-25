/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_buying_power_model.c
 *
 * PURPOSE:
 *   Exercise compute reusable brokerage buying power from cash, equity and margin requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/buying_power_model.h"
int main(void) {

    int64_t b=0;
    if(umi_trading_buying_power_model_compute(100,200,50,&b)!=UMI_STATUS_OK)return 1;
    return b==250?0:2;
}
