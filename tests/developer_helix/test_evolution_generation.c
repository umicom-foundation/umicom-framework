/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_evolution_generation.c
 *
 * PURPOSE:
 *   Regression coverage for collect bounded variants for one generation and track best/mean fitness without automatic deployment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/evolution_generation.h"
int main(void){ UmiHelixEvolutionGeneration g={0}; UmiHelixEvolutionVariant a,b; umi_helix_evolution_variant_init(&a,"a"); a.quality=0.5; a.regression=0.0; umi_helix_evolution_variant_init(&b,"b"); b.quality=0.9; b.regression=0.0; if(umi_helix_evolution_generation_add(&g,&a)!=UMI_STATUS_OK||umi_helix_evolution_generation_add(&g,&b)!=UMI_STATUS_OK||umi_helix_evolution_generation_evaluate(&g)!=UMI_STATUS_OK){ return 1; } if(g.best_index!=1U||g.best_fitness<=g.mean_fitness){ return 2; } return 0; }
