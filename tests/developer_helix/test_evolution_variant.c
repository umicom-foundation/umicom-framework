/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_evolution_variant.c
 *
 * PURPOSE:
 *   Regression coverage for represent a generated implementation variant and its lineage, fitness and validation state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/evolution_variant.h"
int main(void){ UmiHelixEvolutionVariant r; umi_helix_evolution_variant_init(&r,"evolution_variant"); if(umi_helix_evolution_variant_validate(&r)!=UMI_STATUS_OK){ return 1; } r.quality=0.9; r.regression=0.1; double s=umi_helix_evolution_variant_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
