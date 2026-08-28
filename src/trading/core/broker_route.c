/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/broker_route.c
 *
 * PURPOSE:
 *   Describe a candidate broker/venue route with cost and latency scores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route.h"
#include <string.h>
/* Initialise and validate describe a candidate broker/venue route with cost and latency scores.. */
UmiStatus umi_trading_broker_route_init(UmiTradingBrokerRoute *value,const UmiFinancialId * route_id, const UmiFinancialId * venue_id, uint32_t cost_bps, uint32_t latency_score, bool enabled) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    if(route_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->route_id=*route_id;
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->cost_bps=cost_bps;
    value->latency_score=latency_score;
    value->enabled=enabled;
    return umi_trading_broker_route_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_broker_route_valid(const UmiTradingBrokerRoute *value) { return value!=NULL && (value->route_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->cost_bps<=10000U); }
