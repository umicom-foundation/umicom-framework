/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/execution_aggregation.h
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
#ifndef UMICOM_TRADING_CORE_EXECUTION_AGGREGATION_H
#define UMICOM_TRADING_CORE_EXECUTION_AGGREGATION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/execution_fill.h"
/**
 * Represent the trading execution aggregation data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingExecutionAggregation { UmiTradingQuantityLots total_lots; UmiTradingPriceTicks average_price_ticks; uint64_t fill_count; } UmiTradingExecutionAggregation;
/* Initialise an empty execution aggregate. */
void umi_trading_execution_aggregation_init(UmiTradingExecutionAggregation *aggregate);
/* Add one fill and update a quantity-weighted average without wide nonstandard integers. */
/* The average uses the previous rounded average plus a weighted price step;
 * it is an incremental integer estimate, not an exact total-notional ledger.
 * Positive normalized prices/lots are required. Refused input leaves all fields
 * unchanged. Deduplicate execution IDs before adding: this owner keeps no IDs. */
UmiStatus umi_trading_execution_aggregation_add(UmiTradingExecutionAggregation *aggregate,const UmiTradingExecutionFill *fill);
#ifdef __cplusplus
}
#endif
#endif
