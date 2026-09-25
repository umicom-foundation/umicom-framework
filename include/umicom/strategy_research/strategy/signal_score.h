/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/strategy/signal_score.h
 *
 * PURPOSE:
 *   Evaluate Strategy Signal Score as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STRATEGY_SIGNAL_SCORE_H
#define UMICOM_STRATEGY_RESEARCH_STRATEGY_SIGNAL_SCORE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_strategy_signal_score(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
