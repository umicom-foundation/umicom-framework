/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/execution_policy.c
 *
 * PURPOSE:
 *   Define venue-count, participation and urgency bounds for execution strategies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_policy.h"
#include <string.h>
/* Initialise and validate define venue-count, participation and urgency bounds for execution strategies.. */
UmiStatus umi_trading_execution_policy_init(UmiTradingExecutionPolicy *value,uint32_t max_venues, uint32_t participation_bps, uint32_t urgency) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->max_venues=max_venues;
    value->participation_bps=participation_bps;
    value->urgency=urgency;
    return umi_trading_execution_policy_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_execution_policy_valid(const UmiTradingExecutionPolicy *value) { return value!=NULL && (value->max_venues>0U && value->max_venues<=UMI_TRADING_CORE_MAX_ITEMS && value->participation_bps<=10000U && value->urgency<=100U); }
