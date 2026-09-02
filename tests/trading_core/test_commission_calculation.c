/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_commission_calculation.c
 *
 * PURPOSE:
 *   Exercise calculate bounded per-lot brokerage commission from a commission schedule.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/commission_calculation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingCommissionSchedule s;
     int64_t fee=0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_commission_schedule_init(&s,2,10,100)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_commission_calculation_compute(&s,3,&fee)!=UMI_STATUS_OK)return 2;
    return fee==10?0:3;
}
