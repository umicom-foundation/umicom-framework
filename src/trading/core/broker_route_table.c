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
/*
 * Initialise trading broker route table from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_broker_route_table_init(UmiTradingBrokerRouteTable *table){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table!=NULL)memset(table,0,sizeof *table);}
/*
 * Add trading broker route table only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_broker_route_table_add(UmiTradingBrokerRouteTable *table,const UmiTradingBrokerRoute *route){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||!umi_trading_broker_route_valid(route))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<table->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&table->items[i].route_id,&route->route_id))return UMI_STATUS_ALREADY_EXISTS;table->items[table->count++]=*route;return UMI_STATUS_OK;}
/*
 * Provide the trading broker route table best operation used by this module and its client
 * applications.
 */
const UmiTradingBrokerRoute *umi_trading_broker_route_table_best(const UmiTradingBrokerRouteTable *table){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL)return NULL;const UmiTradingBrokerRoute *best=NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<table->count;i++){const UmiTradingBrokerRoute *r=&table->items[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!r->enabled)continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(best==NULL||r->cost_bps<best->cost_bps||(r->cost_bps==best->cost_bps&&r->latency_score<best->latency_score))best=r;}return best;}
