/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/smart_order_router.c
 *
 * PURPOSE:
 *   Choose broker routes using cost, latency and deterministic preference weights.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/smart_order_router.h"

#include <stdint.h>
const UmiTradingBrokerRoute *umi_trading_smart_order_router_select(const UmiTradingBrokerRouteTable *table,const UmiTradingSmartOrderRouterPolicy *policy){if(table==NULL||policy==NULL||(policy->cost_weight==0U&&policy->latency_weight==0U))return NULL;const UmiTradingBrokerRoute *best=NULL;uint64_t best_score=UINT64_MAX;for(size_t i=0;i<table->count;i++){const UmiTradingBrokerRoute *r=&table->items[i];if(!r->enabled)continue;uint64_t score=(uint64_t)r->cost_bps*policy->cost_weight+(uint64_t)r->latency_score*policy->latency_weight;if(score<best_score){best_score=score;best=r;}}return best;}
