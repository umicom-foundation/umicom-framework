/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/matching_engine.h
 *
 * PURPOSE:
 *   Match crossing buy and sell interest under a reusable price-time policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_MATCHING_ENGINE_H
#define UMICOM_TRADING_CORE_MATCHING_ENGINE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/matching_policy.h"
typedef struct UmiTradingMatchRequest { UmiTradingPriceTicks buy_limit_ticks; UmiTradingQuantityLots buy_lots; UmiTradingPriceTicks sell_limit_ticks; UmiTradingQuantityLots sell_lots; } UmiTradingMatchRequest;
typedef struct UmiTradingMatchResult { UmiTradingPriceTicks execution_ticks; UmiTradingQuantityLots execution_lots; bool matched; } UmiTradingMatchResult;
/* Match two opposing limit interests when their prices cross. */
UmiStatus umi_trading_matching_engine_match(const UmiTradingMatchingPolicy *policy,const UmiTradingMatchRequest *request,UmiTradingMatchResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
