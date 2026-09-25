/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/optimisation_engine.c
 *
 * PURPOSE:
 *   Implement deterministic strategy candidate scoring and ranking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/optimisation_engine.h"

#include <string.h>

static double clamp(double value)
{
    if (value < 0.0) return 0.0;
    if (value > 100.0) return 100.0;
    return value;
}

UmiStatus umi_strategy_optimisation_candidate_score(
    UmiStrategyOptimisationCandidate *candidate)
{
    double generalisation;
    double drawdownQuality;

    if (candidate == NULL ||
        candidate->parameterCount >
            UMI_STRATEGY_OPTIMISATION_PARAMETER_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    generalisation =
        100.0 -
        (candidate->trainingScore >= candidate->validationScore
             ? candidate->trainingScore - candidate->validationScore
             : candidate->validationScore - candidate->trainingScore);
    drawdownQuality = 100.0 - candidate->drawdownPercent;

    candidate->objectiveScore = clamp(
        clamp(candidate->validationScore) * 0.40 +
        clamp(candidate->stabilityScore) * 0.25 +
        clamp(generalisation) * 0.20 +
        clamp(drawdownQuality) * 0.15);
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_optimisation_set_add(
    UmiStrategyOptimisationSet *set,
    const UmiStrategyOptimisationCandidate *candidate)
{
    UmiStrategyOptimisationCandidate copy;
    UmiStatus status;

    if (set == NULL || candidate == NULL ||
        set->count >= UMI_STRATEGY_OPTIMISATION_CANDIDATE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *candidate;
    status = umi_strategy_optimisation_candidate_score(&copy);
    if (status != UMI_STATUS_OK) return status;
    set->candidates[set->count++] = copy;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_optimisation_rank(
    UmiStrategyOptimisationSet *set)
{
    size_t index;

    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 1U; index < set->count; ++index) {
        UmiStrategyOptimisationCandidate value = set->candidates[index];
        size_t position = index;

        while (position > 0U &&
               set->candidates[position - 1U].objectiveScore <
                   value.objectiveScore) {
            set->candidates[position] = set->candidates[position - 1U];
            --position;
        }
        set->candidates[position] = value;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_optimisation_best(
    const UmiStrategyOptimisationSet *set,
    UmiStrategyOptimisationCandidate *outCandidate)
{
    if (set == NULL || outCandidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (set->count == 0U) return UMI_STATUS_NOT_FOUND;
    *outCandidate = set->candidates[0];
    return UMI_STATUS_OK;
}
