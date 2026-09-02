/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_selection_policy.c
 *
 * PURPOSE:
 *   Regression coverage for select validated evolution variants deterministically while preserving explicit risk constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/selection_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixEvolutionGeneration g={0}; UmiHelixEvolutionVariant v; umi_helix_evolution_variant_init(&v,"v"); v.quality=0.9; v.regression=0.0; v.state=UMI_HELIX_SUCCEEDED; (void)umi_helix_evolution_generation_add(&g,&v); UmiHelixSelectionPolicy p={0.7,0.5,true}; size_t i=99; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_selection_policy_choose(&p,&g,&i)!=UMI_STATUS_OK||i!=0U){ return 1; } return 0; }
