/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/trade_capture.h
 *
 * PURPOSE:
 *   Capture an exchange match as immutable trade evidence with unique identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_TRADE_CAPTURE_H
#define UMICOM_TRADING_CORE_TRADE_CAPTURE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading trade capture data shared with callers of this public contract.
 */
typedef struct UmiTradingTradeCapture { UmiFinancialId trade_id; UmiFinancialId buy_order_id; UmiFinancialId sell_order_id; UmiTradingPriceTicks price_ticks; UmiTradingQuantityLots quantity_lots; int64_t event_time_ms; } UmiTradingTradeCapture;
/**
 * Initialise trading trade capture from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_trading_trade_capture_init(UmiTradingTradeCapture *trade,const char *trade_id,const UmiFinancialId *buy_order_id,const UmiFinancialId *sell_order_id,UmiTradingPriceTicks price,UmiTradingQuantityLots quantity,int64_t event_time_ms);
/**
 * Check that trading trade capture satisfies its contract before another service relies on
 * it.
 */
bool umi_trading_trade_capture_valid(const UmiTradingTradeCapture *trade);
#ifdef __cplusplus
}
#endif
#endif
