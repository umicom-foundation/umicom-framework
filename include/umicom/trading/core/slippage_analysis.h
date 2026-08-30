/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/slippage_analysis.h
 *
 * PURPOSE:
 *   Aggregate execution slippage in integer ticks with side-aware sign.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_SLIPPAGE_ANALYSIS_H
#define UMICOM_TRADING_CORE_SLIPPAGE_ANALYSIS_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingSlippageAnalysis { int64_t total_slippage_ticks; uint64_t observations; } UmiTradingSlippageAnalysis;
void umi_trading_slippage_analysis_init(UmiTradingSlippageAnalysis *analysis);
UmiStatus umi_trading_slippage_analysis_add(UmiTradingSlippageAnalysis *analysis,UmiSide side,UmiTradingPriceTicks expected,UmiTradingPriceTicks actual);
int64_t umi_trading_slippage_analysis_average(const UmiTradingSlippageAnalysis *analysis);
#ifdef __cplusplus
}
#endif
#endif
