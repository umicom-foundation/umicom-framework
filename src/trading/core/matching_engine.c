/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/matching_engine.c
 *
 * PURPOSE:
 *   Match crossing buy and sell interest under a reusable price-time policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/matching_engine.h"

#include <string.h>
UmiStatus umi_trading_matching_engine_match(const UmiTradingMatchingPolicy *policy,const UmiTradingMatchRequest *request,UmiTradingMatchResult *out_result){if(!umi_trading_matching_policy_valid(policy)||request==NULL||out_result==NULL||request->buy_limit_ticks<=0||request->sell_limit_ticks<=0||request->buy_lots<=0||request->sell_lots<=0)return UMI_STATUS_INVALID_ARGUMENT;memset(out_result,0,sizeof *out_result);if(request->buy_limit_ticks<request->sell_limit_ticks)return UMI_STATUS_NOT_FOUND;out_result->execution_ticks=request->sell_limit_ticks;out_result->execution_lots=request->buy_lots<request->sell_lots?request->buy_lots:request->sell_lots;out_result->matched=true;return UMI_STATUS_OK;}
