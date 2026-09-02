/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/convergence_policy.h
 *
 * PURPOSE:
 *   Determine whether evolution should stop due to target fitness, stagnation, budget or generation limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_CONVERGENCE_POLICY_H
#define UMICOM_DEVELOPER_HELIX_CONVERGENCE_POLICY_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named helix convergence reason values accepted by this public contract.
 */
typedef enum UmiHelixConvergenceReason { UMI_HELIX_CONVERGENCE_CONTINUE=0, UMI_HELIX_CONVERGENCE_TARGET=1, UMI_HELIX_CONVERGENCE_STAGNATION=2, UMI_HELIX_CONVERGENCE_BUDGET=3, UMI_HELIX_CONVERGENCE_GENERATION_LIMIT=4 } UmiHelixConvergenceReason;
/**
 * Represent the helix convergence policy data shared with callers of this public contract.
 */
typedef struct UmiHelixConvergencePolicy { double target_fitness; uint32_t max_stagnant_generations; uint32_t max_generations; uint64_t max_cost; } UmiHelixConvergencePolicy;
/* Evaluate all explicit stopping conditions without selecting or deploying a variant. */
UmiHelixConvergenceReason umi_helix_convergence_policy_evaluate(const UmiHelixConvergencePolicy *policy,double best_fitness,uint32_t stagnant,uint32_t generation,uint64_t cost);
#ifdef __cplusplus
}
#endif
#endif
