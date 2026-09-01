/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/fitness.c
 *
 * PURPOSE:
 *   Calculate a balanced Helix candidate fitness score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Correctness, security and compatibility also have minimum floors so a high average cannot compensate for a critical weakness.
 */

#include "umicom/helix/fitness.h"
#include <stddef.h>

static int valid_score(double value)
{
    return value >= 0.0 && value <= 1.0;
}

UmiStatus umi_helix_fitness_calculate(UmiHelixFitness *fitness,
                                      double correctness,
                                      double security,
                                      double performance,
                                      double maintainability,
                                      double compatibility,
                                      double pass_threshold)
{
    if (fitness == NULL || !valid_score(correctness) || !valid_score(security) ||
        !valid_score(performance) || !valid_score(maintainability) ||
        !valid_score(compatibility) || !valid_score(pass_threshold)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    fitness->correctness = correctness;
    fitness->security = security;
    fitness->performance = performance;
    fitness->maintainability = maintainability;
    fitness->compatibility = compatibility;
    fitness->total = correctness * 0.35 +
                     security * 0.25 +
                     performance * 0.10 +
                     maintainability * 0.15 +
                     compatibility * 0.15;
    fitness->passed = fitness->total >= pass_threshold &&
                      correctness >= 0.80 &&
                      security >= 0.80 &&
                      compatibility >= 0.80;
    return UMI_STATUS_OK;
}
