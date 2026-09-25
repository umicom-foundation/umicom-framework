/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/backtest/exposure.h
 *
 * PURPOSE:
 *   Evaluate Backtest Exposure as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_BACKTEST_EXPOSURE_H
#define UMICOM_STRATEGY_RESEARCH_BACKTEST_EXPOSURE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_backtest_exposure(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
