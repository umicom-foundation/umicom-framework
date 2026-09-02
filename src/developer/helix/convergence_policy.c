/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/convergence_policy.c
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
#include "umicom/developer/helix/convergence_policy.h"
/* Stop evolution as soon as any configured safety/budget/fitness boundary is reached. */
UmiHelixConvergenceReason umi_helix_convergence_policy_evaluate(const UmiHelixConvergencePolicy *p,double best,uint32_t stagnant,uint32_t generation,uint64_t cost){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL){ return UMI_HELIX_CONVERGENCE_BUDGET; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(best>=p->target_fitness){ return UMI_HELIX_CONVERGENCE_TARGET; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->max_stagnant_generations>0U&&stagnant>=p->max_stagnant_generations){ return UMI_HELIX_CONVERGENCE_STAGNATION; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->max_generations>0U&&generation>=p->max_generations){ return UMI_HELIX_CONVERGENCE_GENERATION_LIMIT; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->max_cost>0U&&cost>=p->max_cost){ return UMI_HELIX_CONVERGENCE_BUDGET; } return UMI_HELIX_CONVERGENCE_CONTINUE; }
