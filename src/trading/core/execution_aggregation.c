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
/* Retain the earlier arithmetic for review. Its signed product could reject
 * otherwise representable averages, and corrupt retained state could supply a
 * zero divisor. The replacement preflights the owner and divides an unsigned
 * product bit by bit before committing the new aggregate. */
#if 0
/* Add one fill and update a quantity-weighted average without wide nonstandard integers. */
UmiStatus umi_trading_execution_aggregation_add(UmiTradingExecutionAggregation *aggregate,const UmiTradingExecutionFill *fill){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(aggregate==NULL||!umi_trading_execution_fill_valid(fill))return UMI_STATUS_INVALID_ARGUMENT;int64_t new_total=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_add_i64(aggregate->total_lots,fill->quantity_lots,&new_total)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(aggregate->fill_count==0U){aggregate->average_price_ticks=fill->price_ticks;}/* Use this fallback path when the earlier condition does not apply. */ else{int64_t delta=fill->price_ticks-aggregate->average_price_ticks;int64_t weight=fill->quantity_lots;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(delta!=0&&weight>INT64_MAX/(delta>0?delta:-delta))return UMI_STATUS_CAPACITY_EXCEEDED;aggregate->average_price_ticks+= (delta*weight)/new_total;}aggregate->total_lots=new_total;aggregate->fill_count++;return UMI_STATUS_OK;}

#endif

/* Compute floor(a*b/divisor) for a <= INT64_MAX and b <= divisor <= INT64_MAX.
 * Every partial remainder is below divisor. Doubling and adding b therefore fit
 * in uint64_t even when the full product would need twice its width. This keeps
 * the existing incremental rounding rule portable across C23 compilers. */
static uint64_t ExecutionWeightedStep(uint64_t a, uint64_t b, uint64_t divisor)
{
    uint64_t quotient = 0U, remainder = 0U;
    for (unsigned bit = 63U; bit != 0U; --bit) {
        unsigned shift = bit - 1U;
        remainder *= 2U;
        quotient *= 2U;
        if (remainder >= divisor) { remainder -= divisor; ++quotient; }
        if (((a >> shift) & UINT64_C(1)) != 0U) {
            remainder += b;
            if (remainder >= divisor) { remainder -= divisor; ++quotient; }
        }
    }
    return quotient;
}
UmiStatus umi_trading_execution_aggregation_add(UmiTradingExecutionAggregation *aggregate,
    const UmiTradingExecutionFill *fill)
{
    if (aggregate == NULL || !umi_trading_execution_fill_valid(fill))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (aggregate->fill_count == 0U) {
        if (aggregate->total_lots != 0 || aggregate->average_price_ticks != 0)
            return UMI_STATUS_INVALID_STATE;
    } else if (aggregate->total_lots <= 0 || aggregate->average_price_ticks <= 0 ||
        aggregate->fill_count > (uint64_t)aggregate->total_lots) {
        return UMI_STATUS_INVALID_STATE;
    }
    int64_t total = 0;
    if (aggregate->fill_count == UINT64_MAX ||
        umi_trading_core_add_i64(aggregate->total_lots, fill->quantity_lots, &total) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTradingExecutionAggregation candidate = *aggregate;
    if (candidate.fill_count == 0U) candidate.average_price_ticks = fill->price_ticks;
    else {
        /* Both prices are positive, so the difference and its magnitude fit.
         * The weighted step cannot move the average beyond the new fill price. */
        int64_t delta = fill->price_ticks - candidate.average_price_ticks;
        uint64_t magnitude = (uint64_t)(delta < 0 ? -delta : delta);
        int64_t step = (int64_t)ExecutionWeightedStep(magnitude,
            (uint64_t)fill->quantity_lots, (uint64_t)total);
        candidate.average_price_ticks += delta < 0 ? -step : step;
    }
    candidate.total_lots = total;
    ++candidate.fill_count;
    *aggregate = candidate;
    return UMI_STATUS_OK;
}
