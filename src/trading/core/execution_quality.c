/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/execution_quality.c
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
#include "umicom/trading/core/execution_quality.h"

UmiStatus umi_trading_execution_quality_compute(UmiSide side,UmiTradingPriceTicks reference_ticks,UmiTradingPriceTicks execution_ticks,UmiTradingExecutionQuality *out_quality){if(out_quality==NULL||reference_ticks<=0||execution_ticks<=0||(side!=UMI_SIDE_BUY&&side!=UMI_SIDE_SELL))return UMI_STATUS_INVALID_ARGUMENT;out_quality->reference_ticks=reference_ticks;out_quality->execution_ticks=execution_ticks;out_quality->signed_improvement_ticks=side==UMI_SIDE_BUY?reference_ticks-execution_ticks:execution_ticks-reference_ticks;return UMI_STATUS_OK;}
