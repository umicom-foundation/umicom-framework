/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_optimisation_engine.c
 *
 * PURPOSE:
 *   Verify deterministic candidate scoring, ranking and best-candidate retrieval.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/strategy_research/optimisation_engine.h"

int main(void)
{
    UmiStrategyOptimisationSet set = {0};
    UmiStrategyOptimisationCandidate a = {0};
    UmiStrategyOptimisationCandidate b = {0};
    UmiStrategyOptimisationCandidate best;

    (void)strcpy(a.id, "candidate.a");
    a.parameterCount = 2U;
    a.trainingScore = 90.0;
    a.validationScore = 65.0;
    a.drawdownPercent = 15.0;
    a.stabilityScore = 60.0;

    (void)strcpy(b.id, "candidate.b");
    b.parameterCount = 2U;
    b.trainingScore = 80.0;
    b.validationScore = 78.0;
    b.drawdownPercent = 10.0;
    b.stabilityScore = 82.0;

    assert(umi_strategy_optimisation_set_add(&set, &a) == UMI_STATUS_OK);
    assert(umi_strategy_optimisation_set_add(&set, &b) == UMI_STATUS_OK);
    assert(umi_strategy_optimisation_rank(&set) == UMI_STATUS_OK);
    assert(umi_strategy_optimisation_best(&set, &best) == UMI_STATUS_OK);
    assert(strcmp(best.id, "candidate.b") == 0);
    assert(best.objectiveScore > a.objectiveScore);
    return 0;
}
