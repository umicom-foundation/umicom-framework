/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/tick_size_rule.c
 *
 * PURPOSE:
 *   Represent a price range and legal tick increment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/tick_size_rule.h"
#include <string.h>
/* Initialise and validate represent a price range and legal tick increment.. */
UmiStatus umi_trading_tick_size_rule_init(UmiTradingTickSizeRule *value,UmiTradingPriceTicks minimum_price, UmiTradingPriceTicks maximum_price, UmiTradingPriceTicks tick_size) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->minimum_price=minimum_price;
    value->maximum_price=maximum_price;
    value->tick_size=tick_size;
    return umi_trading_tick_size_rule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_tick_size_rule_valid(const UmiTradingTickSizeRule *value) { return value!=NULL && (value->minimum_price>=0 && value->maximum_price>=value->minimum_price && value->tick_size>0); }
/* Calculate whether a representative price is tick aligned. */
bool umi_trading_tick_size_rule_aligned(const UmiTradingTickSizeRule *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL) return (bool)0; return (value->minimum_price % value->tick_size)==0; }
