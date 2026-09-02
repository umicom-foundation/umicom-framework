/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/evolution_generation.c
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
#include "umicom/developer/helix/evolution_generation.h"
/* Copy a candidate variant into the bounded generation. */
UmiStatus umi_helix_evolution_generation_add(UmiHelixEvolutionGeneration *g,const UmiHelixEvolutionVariant *v){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||v==NULL||umi_helix_evolution_variant_validate(v)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } g->variants[g->count++]=*v; return UMI_STATUS_OK; }
/* Calculate deterministic best/mean fitness over current variants. */
UmiStatus umi_helix_evolution_generation_evaluate(UmiHelixEvolutionGeneration *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||g->count==0U){ return UMI_STATUS_INVALID_ARGUMENT; } double total=0.0,best=-1.0; size_t best_i=0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<g->count;i++){ double s=umi_helix_evolution_variant_score(&g->variants[i]); total+=s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s>best){best=s;best_i=i;} } g->best_fitness=best; g->mean_fitness=total/(double)g->count; g->best_index=best_i; return UMI_STATUS_OK; }
