/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/execution_aggregation.c
 *
 * PURPOSE:
 *   Aggregate fills with overflow-aware quantities and average prices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_aggregation.h"

#include <limits.h>
/*
 * Initialise trading execution aggregation from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_execution_aggregation_init(UmiTradingExecutionAggregation *aggregate){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(aggregate!=NULL){aggregate->total_lots=0;aggregate->average_price_ticks=0;aggregate->fill_count=0U;}}
/* Add one fill and update a quantity-weighted average without wide nonstandard integers. */
UmiStatus umi_trading_execution_aggregation_add(UmiTradingExecutionAggregation *aggregate,const UmiTradingExecutionFill *fill){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(aggregate==NULL||!umi_trading_execution_fill_valid(fill))return UMI_STATUS_INVALID_ARGUMENT;int64_t new_total=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_add_i64(aggregate->total_lots,fill->quantity_lots,&new_total)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(aggregate->fill_count==0U){aggregate->average_price_ticks=fill->price_ticks;}/* Use this fallback path when the earlier condition does not apply. */ else{int64_t delta=fill->price_ticks-aggregate->average_price_ticks;int64_t weight=fill->quantity_lots;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(delta!=0&&weight>INT64_MAX/(delta>0?delta:-delta))return UMI_STATUS_CAPACITY_EXCEEDED;aggregate->average_price_ticks+= (delta*weight)/new_total;}aggregate->total_lots=new_total;aggregate->fill_count++;return UMI_STATUS_OK;}
