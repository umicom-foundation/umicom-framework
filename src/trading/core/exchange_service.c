/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/exchange_service.c
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
#include "umicom/trading/core/exchange_service.h"

#include <string.h>
UmiStatus umi_trading_exchange_service_init(UmiTradingExchangeService *service,const UmiFinancialId *venue_id){if(service==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(service,0,sizeof *service);UmiStatus st=umi_trading_exchange_session_init(&service->session,venue_id,1000U,60U);if(st!=UMI_STATUS_OK)return st;umi_trading_limit_order_book_init(&service->book);umi_trading_exchange_trade_log_init(&service->trades);umi_trading_market_integrity_snapshot_update(&service->integrity,0U,0U,0U,0U);service->ready=true;return UMI_STATUS_OK;}
bool umi_trading_exchange_service_refresh(UmiTradingExchangeService *service,int64_t now_ms){if(service==NULL)return false;bool operational=!umi_trading_circuit_breaker_halted(&service->session.breaker,now_ms)&&service->integrity.severity!=UMI_TRADING_CORE_CRITICAL;service->ready=operational;return service->ready;}
