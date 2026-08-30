/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/broker_route_table.c
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
#include "umicom/trading/core/broker_route_table.h"

#include <string.h>
void umi_trading_broker_route_table_init(UmiTradingBrokerRouteTable *table){if(table!=NULL)memset(table,0,sizeof *table);}
UmiStatus umi_trading_broker_route_table_add(UmiTradingBrokerRouteTable *table,const UmiTradingBrokerRoute *route){if(table==NULL||!umi_trading_broker_route_valid(route))return UMI_STATUS_INVALID_ARGUMENT;if(table->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t i=0;i<table->count;i++)if(umi_trading_core_id_equal(&table->items[i].route_id,&route->route_id))return UMI_STATUS_ALREADY_EXISTS;table->items[table->count++]=*route;return UMI_STATUS_OK;}
const UmiTradingBrokerRoute *umi_trading_broker_route_table_best(const UmiTradingBrokerRouteTable *table){if(table==NULL)return NULL;const UmiTradingBrokerRoute *best=NULL;for(size_t i=0;i<table->count;i++){const UmiTradingBrokerRoute *r=&table->items[i];if(!r->enabled)continue;if(best==NULL||r->cost_bps<best->cost_bps||(r->cost_bps==best->cost_bps&&r->latency_score<best->latency_score))best=r;}return best;}
