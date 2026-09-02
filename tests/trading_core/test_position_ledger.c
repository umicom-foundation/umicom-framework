/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_position_ledger.c
 *
 * PURPOSE:
 *   Exercise maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/position_ledger.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingPositionLedger l;
    UmiFinancialId id;
    umi_trading_position_ledger_init(&l);
    umi_trading_core_id_assign(&id,"i");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_position_ledger_apply(&l,&id,UMI_SIDE_BUY,10)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_position_ledger_apply(&l,&id,UMI_SIDE_SELL,3)!=UMI_STATUS_OK)return 2;
    return umi_trading_position_ledger_get(&l,&id)==7?0:3;
}
