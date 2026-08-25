/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_commission_schedule.c
 *
 * PURPOSE:
 *   Exercise define per-lot and minimum brokerage commission in integer minor units.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/commission_schedule.h"
int main(void) {
    UmiTradingCommissionSchedule v;
     if(umi_trading_commission_schedule_init(&v,2,10,1000)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_commission_schedule_valid(&v)) return 2;
     return 0;
}
