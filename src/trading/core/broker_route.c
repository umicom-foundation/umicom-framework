/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/broker_route.c
 *
 * PURPOSE:
 *   Describe a candidate broker/venue route with cost and latency scores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route.h"
#include <string.h>
/* Initialise and validate describe a candidate broker/venue route with cost and latency scores.. */
UmiStatus umi_trading_broker_route_init(UmiTradingBrokerRoute *value,const UmiFinancialId * route_id, const UmiFinancialId * venue_id, uint32_t cost_bps, uint32_t latency_score, bool enabled) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(route_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->route_id=*route_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->cost_bps=cost_bps;
    value->latency_score=latency_score;
    value->enabled=enabled;
    return umi_trading_broker_route_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_broker_route_valid(const UmiTradingBrokerRoute *value) { return value!=NULL && (value->route_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->cost_bps<=10000U); }
