/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_fee_schedule.c
 *
 * PURPOSE:
 *   Exercise define maker/taker exchange fees in minor units per lot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_schedule.h"
int main(void) {
    UmiTradingFeeSchedule v;
     if(umi_trading_fee_schedule_init(&v,1,2,1)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_fee_schedule_valid(&v)) return 2;
     return 0;
}
