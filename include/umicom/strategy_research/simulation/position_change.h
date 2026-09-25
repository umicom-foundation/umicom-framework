/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/simulation/position_change.h
 *
 * PURPOSE:
 *   Evaluate Simulation Position Change as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SIMULATION_POSITION_CHANGE_H
#define UMICOM_STRATEGY_RESEARCH_SIMULATION_POSITION_CHANGE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_simulation_position_change(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
