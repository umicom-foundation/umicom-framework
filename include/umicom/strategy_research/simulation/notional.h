/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/simulation/notional.h
 *
 * PURPOSE:
 *   Evaluate Simulation Notional as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SIMULATION_NOTIONAL_H
#define UMICOM_STRATEGY_RESEARCH_SIMULATION_NOTIONAL_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_simulation_notional(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
