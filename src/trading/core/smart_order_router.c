/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/smart_order_router.c
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
#include "umicom/trading/core/smart_order_router.h"

#include <stdint.h>
/*
 * Provide the trading smart order router select operation used by this module and its
 * client applications.
 */
const UmiTradingBrokerRoute *umi_trading_smart_order_router_select(const UmiTradingBrokerRouteTable *table,const UmiTradingSmartOrderRouterPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||policy==NULL||(policy->cost_weight==0U&&policy->latency_weight==0U))return NULL;const UmiTradingBrokerRoute *best=NULL;uint64_t best_score=UINT64_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<table->count;i++){const UmiTradingBrokerRoute *r=&table->items[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!r->enabled)continue;uint64_t score=(uint64_t)r->cost_bps*policy->cost_weight+(uint64_t)r->latency_score*policy->latency_weight;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(score<best_score){best_score=score;best=r;}}return best;}
