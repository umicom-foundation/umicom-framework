/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/simulation/fill_ratio.h
 *
 * PURPOSE:
 *   Evaluate Simulation Fill Ratio as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SIMULATION_FILL_RATIO_H
#define UMICOM_STRATEGY_RESEARCH_SIMULATION_FILL_RATIO_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_simulation_fill_ratio(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
