/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/simulation/provenance.h
 *
 * PURPOSE:
 *   Evaluate Simulation Provenance as deterministic research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_SIMULATION_PROVENANCE_H
#define UMICOM_STRATEGY_RESEARCH_SIMULATION_PROVENANCE_H

#include "umicom/strategy_research/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_strategy_research_evaluate_simulation_provenance(
    const UmiStrategyResearchInput *input,
    UmiStrategyResearchSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
