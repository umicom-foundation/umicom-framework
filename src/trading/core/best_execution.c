/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/best_execution.c
 *
 * PURPOSE:
 *   Score execution outcomes using price, explicit cost and latency evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/best_execution.h"

UmiStatus umi_trading_best_execution_score(int64_t improvement_ticks,int64_t explicit_cost_minor,uint32_t latency_score,UmiTradingBestExecutionScore *out_score){if(out_score==NULL||explicit_cost_minor<0)return UMI_STATUS_INVALID_ARGUMENT;out_score->price_component=improvement_ticks;out_score->cost_component=-explicit_cost_minor;out_score->latency_component=-(int64_t)latency_score;int64_t partial=0;if(umi_trading_core_add_i64(out_score->price_component,out_score->cost_component,&partial)!=UMI_STATUS_OK||umi_trading_core_add_i64(partial,out_score->latency_component,&out_score->total_score)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;}
