/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_position_ledger.c
 *
 * PURPOSE:
 *   Exercise maintain bounded per-instrument lot positions for brokerage and exchange services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/position_ledger.h"
int main(void) {

    UmiTradingPositionLedger l;
    UmiFinancialId id;
    umi_trading_position_ledger_init(&l);
    umi_trading_core_id_assign(&id,"i");
    if(umi_trading_position_ledger_apply(&l,&id,UMI_SIDE_BUY,10)!=UMI_STATUS_OK)return 1;
    if(umi_trading_position_ledger_apply(&l,&id,UMI_SIDE_SELL,3)!=UMI_STATUS_OK)return 2;
    return umi_trading_position_ledger_get(&l,&id)==7?0:3;
}
