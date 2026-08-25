/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_quality_observation.c
 *
 * PURPOSE:
 *   Regression coverage for capture quality, coverage, complexity or conformance evidence used by release/repair gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/quality_observation.h"
int main(void){ UmiHelixQualityObservation r; umi_helix_quality_observation_init(&r,"quality_observation"); if(umi_helix_quality_observation_validate(&r)!=UMI_STATUS_OK){ return 1; } r.quality=0.9; r.regression=0.1; double s=umi_helix_quality_observation_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
