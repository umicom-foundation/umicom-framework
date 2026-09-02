/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/execution_quality.h
 *
 * PURPOSE:
 *   Measure execution price improvement or shortfall against a reference price.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_EXECUTION_QUALITY_H
#define UMICOM_TRADING_CORE_EXECUTION_QUALITY_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading execution quality data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingExecutionQuality { UmiTradingPriceTicks reference_ticks; UmiTradingPriceTicks execution_ticks; int64_t signed_improvement_ticks; } UmiTradingExecutionQuality;
/* Calculate signed improvement where positive is beneficial for the order side. */
UmiStatus umi_trading_execution_quality_compute(UmiSide side,UmiTradingPriceTicks reference_ticks,UmiTradingPriceTicks execution_ticks,UmiTradingExecutionQuality *out_quality);
#ifdef __cplusplus
}
#endif
#endif
