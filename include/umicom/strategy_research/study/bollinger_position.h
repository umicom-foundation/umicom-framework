/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/study/bollinger_position.h
 *
 * PURPOSE:
 *   Evaluate Bollinger Position as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STUDY_BOLLINGER_POSITION_H
#define UMICOM_STRATEGY_RESEARCH_STUDY_BOLLINGER_POSITION_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_study_bollinger_position(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
