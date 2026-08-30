/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_mutation_policy.c
 *
 * PURPOSE:
 *   Regression coverage for bound the number, size and classes of allowed changes when deriving a new variant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/mutation_policy.h"
int main(void){ UmiHelixMutationPolicy r; umi_helix_mutation_policy_init(&r,"mutation_policy"); if(umi_helix_mutation_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.limit=10U; r.used=2U; double s=umi_helix_mutation_policy_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
