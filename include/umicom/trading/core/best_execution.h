/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/best_execution.h
 *
 * PURPOSE:
 *   Score execution outcomes using price, explicit cost and latency evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_BEST_EXECUTION_H
#define UMICOM_TRADING_CORE_BEST_EXECUTION_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingBestExecutionScore { int64_t price_component; int64_t cost_component; int64_t latency_component; int64_t total_score; } UmiTradingBestExecutionScore;
/* Build a deterministic best-execution score where higher is better. */
UmiStatus umi_trading_best_execution_score(int64_t improvement_ticks,int64_t explicit_cost_minor,uint32_t latency_score,UmiTradingBestExecutionScore *out_score);
#ifdef __cplusplus
}
#endif
#endif
