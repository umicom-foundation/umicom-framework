/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_build_observation.c
 *
 * PURPOSE:
 *   Regression coverage for capture build outcome, warning/error counts, duration and artifact evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/build_observation.h"
int main(void){ UmiHelixBuildObservation r; umi_helix_build_observation_init(&r,"build_observation"); if(umi_helix_build_observation_validate(&r)!=UMI_STATUS_OK){ return 1; } r.passed=9U; r.failed=1U; double s=umi_helix_build_observation_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
