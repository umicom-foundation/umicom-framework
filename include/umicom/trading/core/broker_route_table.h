/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/broker_route_table.h
 *
 * PURPOSE:
 *   Maintain candidate broker routes and select stable lowest-cost enabled routes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BROKER_ROUTE_TABLE_H
#define UMICOM_TRADING_CORE_BROKER_ROUTE_TABLE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/broker_route.h"
/**
 * Represent the trading broker route table data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingBrokerRouteTable { UmiTradingBrokerRoute items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingBrokerRouteTable;
/**
 * Initialise trading broker route table from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_broker_route_table_init(UmiTradingBrokerRouteTable *table);
/**
 * Add trading broker route table only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_broker_route_table_add(UmiTradingBrokerRouteTable *table,const UmiTradingBrokerRoute *route);
/**
 * Provide the trading broker route table best operation used by this module and its client
 * applications.
 */
const UmiTradingBrokerRoute *umi_trading_broker_route_table_best(const UmiTradingBrokerRouteTable *table);
#ifdef __cplusplus
}
#endif
#endif
