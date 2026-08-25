/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_validation.c
 *
 * PURPOSE:
 *   Exercise apply shared lot, tick and price-band checks before an order reaches routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_validation.h"
int main(void) {

    UmiTradingOrderIdentity id;
     UmiInstrument inst={0};
     UmiTradingOrderInstruction o;
     UmiTradingTickSizeRule t;
     UmiTradingLotSizeRule l;
     UmiTradingPriceBand b;
    umi_trading_order_identity_init(&id,"c");
    umi_trading_core_id_assign(&inst.instrument_id,"i");
    umi_trading_order_instruction_init(&o,&id,&inst,UMI_SIDE_BUY,UMI_ORDER_LIMIT,UMI_TIF_DAY,10,1000,0);
    umi_trading_tick_size_rule_init(&t,0,10000,5);
    umi_trading_lot_size_rule_init(&l,1,1,100);
    umi_trading_price_band_init(&b,1000,1000U,1000U);
    return umi_trading_order_validation_check(&o,&t,&l,&b).allowed?0:1;
}
