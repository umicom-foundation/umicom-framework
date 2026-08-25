/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_patch_candidate.c
 *
 * PURPOSE:
 *   Regression coverage for represent one candidate repair or regeneration patch before it is selected for application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/patch_candidate.h"
int main(void){ UmiHelixPatchCandidate r; umi_helix_patch_candidate_init(&r,"patch_candidate"); if(umi_helix_patch_candidate_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.9; r.risk=0.1; r.cost=0.2; double s=umi_helix_patch_candidate_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
