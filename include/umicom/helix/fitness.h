/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/fitness.h
 *
 * PURPOSE:
 *   Accumulate weighted correctness, security, performance and maintainability fitness scores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Fitness combines several explicit dimensions so one faster benchmark cannot hide a security or compatibility regression.
 */

#ifndef INCLUDE_UMICOM_HELIX_FITNESS_H
#define INCLUDE_UMICOM_HELIX_FITNESS_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix fitness data shared with callers of this public contract.
 */
typedef struct UmiHelixFitness {
    double correctness;
    double security;
    double performance;
    double maintainability;
    double compatibility;
    double total;
    int passed;
} UmiHelixFitness;

/**
 * Provide the helix fitness calculate operation used by this module and its client
 * applications.
 */
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
