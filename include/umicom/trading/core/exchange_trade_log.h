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
/**
 * Represent the trading exchange trade log data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingExchangeTradeLog { UmiTradingTradeCapture trades[UMI_TRADING_CORE_MAX_EVENTS]; size_t count; uint64_t revision; } UmiTradingExchangeTradeLog;
/**
 * Initialise trading exchange trade log from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_exchange_trade_log_init(UmiTradingExchangeTradeLog *log);
/**
 * Add trading exchange trade log only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_exchange_trade_log_append(UmiTradingExchangeTradeLog *log,const UmiTradingTradeCapture *trade);
/**
 * Find trading exchange trade log while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTradingTradeCapture *umi_trading_exchange_trade_log_find(const UmiTradingExchangeTradeLog *log,const UmiFinancialId *trade_id);
#ifdef __cplusplus
}
#endif
#endif
