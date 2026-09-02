/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_commission_schedule.c
 *
 * PURPOSE:
 *   Exercise define per-lot and minimum brokerage commission in integer minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/commission_schedule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingCommissionSchedule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_commission_schedule_init(&v,2,10,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_commission_schedule_valid(&v)) return 2;
     return 0;
}
