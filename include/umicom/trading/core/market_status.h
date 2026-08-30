/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/market_status.h
 *
 * PURPOSE:
 *   Capture exchange phase, sequence and operational availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MARKET_STATUS_H
#define UMICOM_TRADING_CORE_MARKET_STATUS_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingMarketStatus { UmiTradingCoreMarketPhase phase; uint64_t sequence; bool operational; } UmiTradingMarketStatus;
/* Initialise and validate capture exchange phase, sequence and operational availability. */
UmiStatus umi_trading_market_status_init(UmiTradingMarketStatus *value,UmiTradingCoreMarketPhase phase, uint64_t sequence, bool operational);
/* Validate the invariant set for this trading record. */
bool umi_trading_market_status_valid(const UmiTradingMarketStatus *value);
#ifdef __cplusplus
}
#endif
#endif
