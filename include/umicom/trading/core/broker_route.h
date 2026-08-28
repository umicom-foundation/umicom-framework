/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/broker_route.h
 *
 * PURPOSE:
 *   Describe a candidate broker/venue route with cost and latency scores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKER_ROUTE_H
#define UMICOM_TRADING_CORE_BROKER_ROUTE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingBrokerRoute { UmiFinancialId route_id; UmiFinancialId venue_id; uint32_t cost_bps; uint32_t latency_score; bool enabled; } UmiTradingBrokerRoute;
/* Initialise and validate describe a candidate broker/venue route with cost and latency scores. */
UmiStatus umi_trading_broker_route_init(UmiTradingBrokerRoute *value,const UmiFinancialId * route_id, const UmiFinancialId * venue_id, uint32_t cost_bps, uint32_t latency_score, bool enabled);
/* Validate the invariant set for this trading record. */
bool umi_trading_broker_route_valid(const UmiTradingBrokerRoute *value);
#ifdef __cplusplus
}
#endif
#endif
