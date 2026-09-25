/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/study/support_distance.h
 *
 * PURPOSE:
 *   Evaluate Support Distance as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_STUDY_SUPPORT_DISTANCE_H
#define UMICOM_STRATEGY_RESEARCH_STUDY_SUPPORT_DISTANCE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_study_support_distance(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
