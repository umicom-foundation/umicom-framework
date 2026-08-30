/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_replace.c
 *
 * PURPOSE:
 *   Describe a replacement request with optimistic order-version control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_replace.h"
#include <string.h>
/* Initialise and validate describe a replacement request with optimistic order-version control.. */
UmiStatus umi_trading_order_replace_init(UmiTradingOrderReplace *value,const UmiFinancialId * client_order_id, uint64_t expected_version, UmiTradingQuantityLots new_quantity_lots, UmiTradingPriceTicks new_limit_ticks) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    if(client_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->client_order_id=*client_order_id;
    value->expected_version=expected_version;
    value->new_quantity_lots=new_quantity_lots;
    value->new_limit_ticks=new_limit_ticks;
    return umi_trading_order_replace_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_replace_valid(const UmiTradingOrderReplace *value) { return value!=NULL && (value->client_order_id.value[0]!='\0' && value->expected_version>0U && value->new_quantity_lots>0 && value->new_limit_ticks>=0); }
