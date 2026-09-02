/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/selection_policy.h
 *
 * PURPOSE:
 *   Select validated evolution variants deterministically while preserving explicit risk constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_SELECTION_POLICY_H
#define UMICOM_DEVELOPER_HELIX_SELECTION_POLICY_H
#include "umicom/developer/helix/evolution_generation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix selection policy data shared with callers of this public contract.
 */
typedef struct UmiHelixSelectionPolicy { double minimum_fitness; double maximum_risk; bool require_success; } UmiHelixSelectionPolicy;
/* Select the highest-fitness variant satisfying validation and risk constraints. */
UmiStatus umi_helix_selection_policy_choose(const UmiHelixSelectionPolicy *policy,const UmiHelixEvolutionGeneration *generation,size_t *out_index);
#ifdef __cplusplus
}
#endif
#endif
