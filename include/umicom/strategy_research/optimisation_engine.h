/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/strategy_research/optimisation_engine.h
 *
 * PURPOSE:
 *   Rank bounded strategy parameter candidates using deterministic objective,
 *   validation, drawdown and stability evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRATEGY_RESEARCH_OPTIMISATION_ENGINE_H
#define UMICOM_STRATEGY_RESEARCH_OPTIMISATION_ENGINE_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STRATEGY_OPTIMISATION_PARAMETER_CAPACITY 8U
#define UMI_STRATEGY_OPTIMISATION_CANDIDATE_CAPACITY 128U

typedef struct UmiStrategyOptimisationCandidate {
    char id[64];
    double parameters[UMI_STRATEGY_OPTIMISATION_PARAMETER_CAPACITY];
    size_t parameterCount;
    double trainingScore;
    double validationScore;
    double drawdownPercent;
    double stabilityScore;
    double objectiveScore;
} UmiStrategyOptimisationCandidate;

typedef struct UmiStrategyOptimisationSet {
    UmiStrategyOptimisationCandidate
        candidates[UMI_STRATEGY_OPTIMISATION_CANDIDATE_CAPACITY];
    size_t count;
} UmiStrategyOptimisationSet;

UmiStatus umi_strategy_optimisation_candidate_score(
    UmiStrategyOptimisationCandidate *candidate);

UmiStatus umi_strategy_optimisation_set_add(
    UmiStrategyOptimisationSet *set,
    const UmiStrategyOptimisationCandidate *candidate);

UmiStatus umi_strategy_optimisation_rank(
    UmiStrategyOptimisationSet *set);

UmiStatus umi_strategy_optimisation_best(
    const UmiStrategyOptimisationSet *set,
    UmiStrategyOptimisationCandidate *outCandidate);

#ifdef __cplusplus
}
#endif
#endif
