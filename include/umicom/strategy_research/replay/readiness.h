/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/replay/readiness.h
 *
 * PURPOSE:
 *   Evaluate Replay Readiness as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_REPLAY_READINESS_H
#define UMICOM_STRATEGY_RESEARCH_REPLAY_READINESS_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_replay_readiness(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
