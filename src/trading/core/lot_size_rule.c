/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/lot_size_rule.c
 *
 * PURPOSE:
 *   Represent minimum and step quantities for a venue listing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/lot_size_rule.h"
#include <string.h>
/* Initialise and validate represent minimum and step quantities for a venue listing.. */
UmiStatus umi_trading_lot_size_rule_init(UmiTradingLotSizeRule *value,UmiTradingQuantityLots minimum_lots, UmiTradingQuantityLots step_lots, UmiTradingQuantityLots maximum_lots) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->minimum_lots=minimum_lots;
    value->step_lots=step_lots;
    value->maximum_lots=maximum_lots;
    return umi_trading_lot_size_rule_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_lot_size_rule_valid(const UmiTradingLotSizeRule *value) { return value!=NULL && (value->minimum_lots>0 && value->step_lots>0 && value->maximum_lots>=value->minimum_lots); }
