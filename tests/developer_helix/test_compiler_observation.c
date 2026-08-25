/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_compiler_observation.c
 *
 * PURPOSE:
 *   Regression coverage for capture provider, target, diagnostic and compilation-stage evidence from framework compiler services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/compiler_observation.h"
int main(void){ UmiHelixCompilerObservation r; umi_helix_compiler_observation_init(&r,"compiler_observation"); if(umi_helix_compiler_observation_validate(&r)!=UMI_STATUS_OK){ return 1; } r.passed=9U; r.failed=1U; double s=umi_helix_compiler_observation_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
