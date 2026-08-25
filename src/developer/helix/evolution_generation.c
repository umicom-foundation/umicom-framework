/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/evolution_generation.c
 *
 * PURPOSE:
 *   Collect bounded variants for one generation and track best/mean fitness without automatic deployment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/evolution_generation.h"
/* Copy a candidate variant into the bounded generation. */
UmiStatus umi_helix_evolution_generation_add(UmiHelixEvolutionGeneration *g,const UmiHelixEvolutionVariant *v){ if(g==NULL||v==NULL||umi_helix_evolution_variant_validate(v)!=UMI_STATUS_OK){ return UMI_STATUS_INVALID_ARGUMENT; } if(g->count>=UMI_HELIX_MAX_ITEMS){ return UMI_STATUS_CAPACITY_EXCEEDED; } g->variants[g->count++]=*v; return UMI_STATUS_OK; }
/* Calculate deterministic best/mean fitness over current variants. */
UmiStatus umi_helix_evolution_generation_evaluate(UmiHelixEvolutionGeneration *g){ if(g==NULL||g->count==0U){ return UMI_STATUS_INVALID_ARGUMENT; } double total=0.0,best=-1.0; size_t best_i=0; for(size_t i=0;i<g->count;i++){ double s=umi_helix_evolution_variant_score(&g->variants[i]); total+=s; if(s>best){best=s;best_i=i;} } g->best_fitness=best; g->mean_fitness=total/(double)g->count; g->best_index=best_i; return UMI_STATUS_OK; }
