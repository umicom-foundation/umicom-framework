/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/slippage_analysis.c
 *
 * PURPOSE:
 *   Aggregate execution slippage in integer ticks with side-aware sign.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/slippage_analysis.h"

void umi_trading_slippage_analysis_init(UmiTradingSlippageAnalysis *analysis){if(analysis!=NULL){analysis->total_slippage_ticks=0;analysis->observations=0U;}}
UmiStatus umi_trading_slippage_analysis_add(UmiTradingSlippageAnalysis *analysis,UmiSide side,UmiTradingPriceTicks expected,UmiTradingPriceTicks actual){if(analysis==NULL||expected<=0||actual<=0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;int64_t slip=side==UMI_SIDE_BUY?actual-expected:expected-actual;if(umi_trading_core_add_i64(analysis->total_slippage_ticks,slip,&analysis->total_slippage_ticks)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;analysis->observations++;return UMI_STATUS_OK;}
int64_t umi_trading_slippage_analysis_average(const UmiTradingSlippageAnalysis *analysis){return analysis!=NULL&&analysis->observations>0U?analysis->total_slippage_ticks/(int64_t)analysis->observations:0;}
