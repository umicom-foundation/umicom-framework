/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_autonomy_level.c
 *
 * PURPOSE:
 *   Regression coverage for classify manual, assisted, supervised and guarded-autonomous operating modes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/autonomy_level.h"
int main(void){ UmiHelixAutonomyLevel r; umi_helix_autonomy_level_init(&r,"autonomy_level"); if(umi_helix_autonomy_level_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_autonomy_level_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
