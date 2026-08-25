/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_risk_policy.c
 *
 * PURPOSE:
 *   Regression coverage for define severity thresholds that force review, denial or rollback during autonomous engineering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/risk_policy.h"
int main(void){ UmiHelixRiskPolicy r; umi_helix_risk_policy_init(&r,"risk_policy"); if(umi_helix_risk_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_risk_policy_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
