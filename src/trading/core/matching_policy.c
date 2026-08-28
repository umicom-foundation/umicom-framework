/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/matching_policy.c
 *
 * PURPOSE:
 *   Define common exchange matching priorities and self-trade prevention behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/matching_policy.h"
#include <string.h>
/* Initialise and validate define common exchange matching priorities and self-trade prevention behaviour.. */
UmiStatus umi_trading_matching_policy_init(UmiTradingMatchingPolicy *value,bool price_time_priority, bool prevent_self_trade, uint32_t max_matches_per_cycle) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->price_time_priority=price_time_priority;
    value->prevent_self_trade=prevent_self_trade;
    value->max_matches_per_cycle=max_matches_per_cycle;
    return umi_trading_matching_policy_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_matching_policy_valid(const UmiTradingMatchingPolicy *value) { return value!=NULL && (value->price_time_priority && value->max_matches_per_cycle>0U && value->max_matches_per_cycle<=UMI_TRADING_CORE_MAX_EVENTS); }
