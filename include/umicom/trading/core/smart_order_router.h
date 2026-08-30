/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/smart_order_router.h
 *
 * PURPOSE:
 *   Choose broker routes using cost, latency and deterministic preference weights.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_SMART_ORDER_ROUTER_H
#define UMICOM_TRADING_CORE_SMART_ORDER_ROUTER_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/broker_route_table.h"
typedef struct UmiTradingSmartOrderRouterPolicy { uint32_t cost_weight; uint32_t latency_weight; } UmiTradingSmartOrderRouterPolicy;
/* Select the enabled route with the lowest weighted score. */
const UmiTradingBrokerRoute *umi_trading_smart_order_router_select(const UmiTradingBrokerRouteTable *table,const UmiTradingSmartOrderRouterPolicy *policy);
#ifdef __cplusplus
}
#endif
#endif
