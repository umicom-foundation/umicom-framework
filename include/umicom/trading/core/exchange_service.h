/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/exchange_service.h
 *
 * PURPOSE:
 *   Aggregate venue session, order book, trade log and integrity state for thin exchange applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXCHANGE_SERVICE_H
#define UMICOM_TRADING_CORE_EXCHANGE_SERVICE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/exchange_session.h"
#include "umicom/trading/core/limit_order_book.h"
#include "umicom/trading/core/exchange_trade_log.h"
#include "umicom/trading/core/market_integrity_snapshot.h"
/**
 * Represent the trading exchange service data shared with callers of this public contract.
 */
typedef struct UmiTradingExchangeService { UmiTradingExchangeSession session; UmiTradingLimitOrderBook book; UmiTradingExchangeTradeLog trades; UmiTradingMarketIntegritySnapshot integrity; bool ready; } UmiTradingExchangeService;
/**
 * Initialise trading exchange service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_exchange_service_init(UmiTradingExchangeService *service,const UmiFinancialId *venue_id);
/* Recompute readiness from market phase and integrity state. */
bool umi_trading_exchange_service_refresh(UmiTradingExchangeService *service,int64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
