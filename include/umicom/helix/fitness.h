/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/fitness.h
 *
 * PURPOSE:
 *   Accumulate weighted correctness, security, performance and maintainability fitness scores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Fitness combines several explicit dimensions so one faster benchmark cannot hide a security or compatibility regression.
 */

#ifndef INCLUDE_UMICOM_HELIX_FITNESS_H
#define INCLUDE_UMICOM_HELIX_FITNESS_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixFitness {
    double correctness;
    double security;
    double performance;
    double maintainability;
    double compatibility;
    double total;
    int passed;
} UmiHelixFitness;

UmiStatus umi_helix_fitness_calculate(UmiHelixFitness *fitness,
                                      double correctness,
                                      double security,
                                      double performance,
                                      double maintainability,
                                      double compatibility,
                                      double pass_threshold);

#ifdef __cplusplus
}
#endif

#endif
