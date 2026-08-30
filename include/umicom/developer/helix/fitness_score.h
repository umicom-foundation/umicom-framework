/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/fitness_score.h
 *
 * PURPOSE:
 *   Combine correctness, quality, performance, compatibility and change-cost dimensions into a score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_FITNESS_SCORE_H
#define UMICOM_DEVELOPER_HELIX_FITNESS_SCORE_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixFitnessScore { double correctness; double quality; double performance; double compatibility; double change_cost; double total; } UmiHelixFitnessScore;
/* Compute a conservative weighted fitness score with change cost treated as a penalty. */
double umi_helix_fitness_score_compute(UmiHelixFitnessScore *score);
#ifdef __cplusplus
}
#endif
#endif
