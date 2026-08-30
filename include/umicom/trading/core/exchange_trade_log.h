/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/exchange_trade_log.h
 *
 * PURPOSE:
 *   Maintain sequence-stable bounded exchange trade capture evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXCHANGE_TRADE_LOG_H
#define UMICOM_TRADING_CORE_EXCHANGE_TRADE_LOG_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/trade_capture.h"
typedef struct UmiTradingExchangeTradeLog { UmiTradingTradeCapture trades[UMI_TRADING_CORE_MAX_EVENTS]; size_t count; uint64_t revision; } UmiTradingExchangeTradeLog;
void umi_trading_exchange_trade_log_init(UmiTradingExchangeTradeLog *log);
UmiStatus umi_trading_exchange_trade_log_append(UmiTradingExchangeTradeLog *log,const UmiTradingTradeCapture *trade);
const UmiTradingTradeCapture *umi_trading_exchange_trade_log_find(const UmiTradingExchangeTradeLog *log,const UmiFinancialId *trade_id);
#ifdef __cplusplus
}
#endif
#endif
