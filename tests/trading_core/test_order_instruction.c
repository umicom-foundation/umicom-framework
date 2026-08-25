/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_instruction.c
 *
 * PURPOSE:
 *   Exercise normalise an existing order request into integer price-tick and lot instructions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_instruction.h"
int main(void) {

    UmiTradingOrderIdentity id;
     UmiInstrument inst={0};
     UmiTradingOrderInstruction o;
    umi_trading_order_identity_init(&id,"c");
     umi_trading_core_id_assign(&inst.instrument_id,"i");
    if(umi_trading_order_instruction_init(&o,&id,&inst,UMI_SIDE_BUY,UMI_ORDER_LIMIT,UMI_TIF_DAY,10,100,0)!=UMI_STATUS_OK)return 1;
    return umi_trading_order_instruction_valid(&o)?0:2;
}
