/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/slippage_analysis.c
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
#include "umicom/trading/core/slippage_analysis.h"

/*
 * Initialise trading slippage analysis from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_slippage_analysis_init(UmiTradingSlippageAnalysis *analysis){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(analysis!=NULL){analysis->total_slippage_ticks=0;analysis->observations=0U;}}
/*
 * Add trading slippage analysis only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_slippage_analysis_add(UmiTradingSlippageAnalysis *analysis,UmiSide side,UmiTradingPriceTicks expected,UmiTradingPriceTicks actual){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(analysis==NULL||expected<=0||actual<=0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;int64_t slip=side==UMI_SIDE_BUY?actual-expected:expected-actual;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_add_i64(analysis->total_slippage_ticks,slip,&analysis->total_slippage_ticks)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;analysis->observations++;return UMI_STATUS_OK;}
/*
 * Provide the trading slippage analysis average operation used by this module and its
 * client applications.
 */
int64_t umi_trading_slippage_analysis_average(const UmiTradingSlippageAnalysis *analysis){return analysis!=NULL&&analysis->observations>0U?analysis->total_slippage_ticks/(int64_t)analysis->observations:0;}
