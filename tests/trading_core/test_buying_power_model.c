/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_buying_power_model.c
 *
 * PURPOSE:
 *   Exercise compute reusable brokerage buying power from cash, equity and margin requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/buying_power_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    int64_t b=0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_buying_power_model_compute(100,200,50,&b)!=UMI_STATUS_OK)return 1;
    return b==250?0:2;
}
