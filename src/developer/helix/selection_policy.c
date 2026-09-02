/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/selection_policy.c
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
#include "umicom/developer/helix/selection_policy.h"
/* Filter candidates by explicit policy before ranking them by score. */
UmiStatus umi_helix_selection_policy_choose(const UmiHelixSelectionPolicy *p,const UmiHelixEvolutionGeneration *g,size_t *out){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||g==NULL||out==NULL){ return UMI_STATUS_INVALID_ARGUMENT; } bool found=false; double best=-1.0; size_t bi=0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<g->count;i++){ const UmiHelixEvolutionVariant *v=&g->variants[i]; double score=umi_helix_evolution_variant_score(v); double risk=1.0-score; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(score<p->minimum_fitness||risk>p->maximum_risk||(p->require_success&&v->state!=UMI_HELIX_SUCCEEDED)) continue; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!found||score>best){found=true;best=score;bi=i;} } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!found){ return UMI_STATUS_NOT_FOUND; } *out=bi; return UMI_STATUS_OK; }
