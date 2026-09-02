/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_fee_calculation.c
 *
 * PURPOSE:
 *   Exercise calculate maker or taker exchange fees plus regulatory charges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/fee_calculation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingFeeSchedule s;
    int64_t fee=0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_fee_schedule_init(&s,1,2,1)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_fee_calculation_compute(&s,10,false,&fee)!=UMI_STATUS_OK)return 2;
    return fee==30?0:3;
}
