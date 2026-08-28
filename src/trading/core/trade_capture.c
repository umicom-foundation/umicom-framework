/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/trade_capture.c
 *
 * PURPOSE:
 *   Capture an exchange match as immutable trade evidence with unique identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/trade_capture.h"

#include <string.h>
UmiStatus umi_trading_trade_capture_init(UmiTradingTradeCapture *trade,const char *trade_id,const UmiFinancialId *buy_order_id,const UmiFinancialId *sell_order_id,UmiTradingPriceTicks price,UmiTradingQuantityLots quantity,int64_t event_time_ms){if(trade==NULL||buy_order_id==NULL||sell_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(trade,0,sizeof *trade);if(umi_trading_core_id_assign(&trade->trade_id,trade_id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;trade->buy_order_id=*buy_order_id;trade->sell_order_id=*sell_order_id;trade->price_ticks=price;trade->quantity_lots=quantity;trade->event_time_ms=event_time_ms;return umi_trading_trade_capture_valid(trade)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
bool umi_trading_trade_capture_valid(const UmiTradingTradeCapture *trade){return trade!=NULL&&trade->trade_id.value[0]!='\0'&&trade->buy_order_id.value[0]!='\0'&&trade->sell_order_id.value[0]!='\0'&&trade->price_ticks>0&&trade->quantity_lots>0&&trade->event_time_ms>=0;}
