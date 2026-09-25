/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/replay/speed.h
 *
 * PURPOSE:
 *   Evaluate Replay Speed as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_REPLAY_SPEED_H
#define UMICOM_STRATEGY_RESEARCH_REPLAY_SPEED_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_replay_speed(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
