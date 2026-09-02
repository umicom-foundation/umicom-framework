/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/evolution_generation.h
 *
 * PURPOSE:
 *   Collect bounded variants for one generation and track best/mean fitness without automatic deployment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EVOLUTION_GENERATION_H
#define UMICOM_DEVELOPER_HELIX_EVOLUTION_GENERATION_H
#include "umicom/developer/helix/evolution_variant.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the helix evolution generation data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixEvolutionGeneration { UmiHelixEvolutionVariant variants[UMI_HELIX_MAX_ITEMS]; size_t count; uint32_t generation; double best_fitness; double mean_fitness; size_t best_index; } UmiHelixEvolutionGeneration;
/* Add one validated implementation variant. */
UmiStatus umi_helix_evolution_generation_add(UmiHelixEvolutionGeneration *generation,const UmiHelixEvolutionVariant *variant);
/* Recompute best and mean validated fitness without deploying a variant. */
UmiStatus umi_helix_evolution_generation_evaluate(UmiHelixEvolutionGeneration *generation);
#ifdef __cplusplus
}
#endif
#endif
