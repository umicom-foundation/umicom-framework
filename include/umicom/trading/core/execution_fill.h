/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/execution_fill.h
 *
 * PURPOSE:
 *   Represent one integer-normalised venue fill for deterministic aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXECUTION_FILL_H
#define UMICOM_TRADING_CORE_EXECUTION_FILL_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading execution fill data shared with callers of this public contract.
 */
typedef struct UmiTradingExecutionFill { UmiFinancialId execution_id; UmiFinancialId client_order_id; UmiTradingQuantityLots quantity_lots; UmiTradingPriceTicks price_ticks; int64_t event_time_ms; } UmiTradingExecutionFill;
/* Initialise and validate represent one integer-normalised venue fill for deterministic aggregation. */
UmiStatus umi_trading_execution_fill_init(UmiTradingExecutionFill *value,const UmiFinancialId * execution_id, const UmiFinancialId * client_order_id, UmiTradingQuantityLots quantity_lots, UmiTradingPriceTicks price_ticks, int64_t event_time_ms);
/* Validate the invariant set for this trading record. */
bool umi_trading_execution_fill_valid(const UmiTradingExecutionFill *value);
#ifdef __cplusplus
}
#endif
#endif
