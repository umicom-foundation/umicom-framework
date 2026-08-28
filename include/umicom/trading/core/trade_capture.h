/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/trade_capture.h
 *
 * PURPOSE:
 *   Capture an exchange match as immutable trade evidence with unique identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TRADE_CAPTURE_H
#define UMICOM_TRADING_CORE_TRADE_CAPTURE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingTradeCapture { UmiFinancialId trade_id; UmiFinancialId buy_order_id; UmiFinancialId sell_order_id; UmiTradingPriceTicks price_ticks; UmiTradingQuantityLots quantity_lots; int64_t event_time_ms; } UmiTradingTradeCapture;
UmiStatus umi_trading_trade_capture_init(UmiTradingTradeCapture *trade,const char *trade_id,const UmiFinancialId *buy_order_id,const UmiFinancialId *sell_order_id,UmiTradingPriceTicks price,UmiTradingQuantityLots quantity,int64_t event_time_ms);
bool umi_trading_trade_capture_valid(const UmiTradingTradeCapture *trade);
#ifdef __cplusplus
}
#endif
#endif
