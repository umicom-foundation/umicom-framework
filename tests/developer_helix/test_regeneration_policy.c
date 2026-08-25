/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_regeneration_policy.c
 *
 * PURPOSE:
 *   Regression coverage for define when regeneration is allowed, reviewable, blocked or forced back to manual engineering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/regeneration_policy.h"
int main(void){ UmiHelixRegenerationPolicy r; umi_helix_regeneration_policy_init(&r,"regeneration_policy"); if(umi_helix_regeneration_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_regeneration_policy_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
