/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/broker_route_table.h
 *
 * PURPOSE:
 *   Maintain candidate broker routes and select stable lowest-cost enabled routes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKER_ROUTE_TABLE_H
#define UMICOM_TRADING_CORE_BROKER_ROUTE_TABLE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/broker_route.h"
typedef struct UmiTradingBrokerRouteTable { UmiTradingBrokerRoute items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingBrokerRouteTable;
void umi_trading_broker_route_table_init(UmiTradingBrokerRouteTable *table);
UmiStatus umi_trading_broker_route_table_add(UmiTradingBrokerRouteTable *table,const UmiTradingBrokerRoute *route);
const UmiTradingBrokerRoute *umi_trading_broker_route_table_best(const UmiTradingBrokerRouteTable *table);
#ifdef __cplusplus
}
#endif
#endif
