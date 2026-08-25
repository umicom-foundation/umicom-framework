/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_cost.c
 *
 * PURPOSE:
 *   Estimate deterministic relative query cost for policy and routing decisions without backend execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_cost.h"
#include <string.h>

/* Cost is intentionally relative; backend adapters may layer native EXPLAIN evidence later. */ UmiStatus umi_data_query_cost_estimate(const UmiDataQueryPlan *plan,double expensive_threshold,UmiDataQueryCost *out_cost){double base;if(plan==NULL||out_cost==NULL||expensive_threshold<=0.0)return UMI_STATUS_INVALID_ARGUMENT;if(umi_data_query_plan_validate(plan)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;base=1.0+(double)plan->projection_count*0.25+(double)plan->order_count*0.5;out_cost->join_penalty=(double)plan->join_count*4.0;out_cost->predicate_discount=(double)plan->predicate_count*0.2;if(out_cost->predicate_discount>base*0.5)out_cost->predicate_discount=base*0.5;out_cost->estimated_cost=base+out_cost->join_penalty-out_cost->predicate_discount;out_cost->expensive=out_cost->estimated_cost>=expensive_threshold;return UMI_STATUS_OK;}
