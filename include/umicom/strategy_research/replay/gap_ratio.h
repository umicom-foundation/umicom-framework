/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/replay/gap_ratio.h
 *
 * PURPOSE:
 *   Evaluate Replay Gap Ratio as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_REPLAY_GAP_RATIO_H
#define UMICOM_STRATEGY_RESEARCH_REPLAY_GAP_RATIO_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_replay_gap_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
