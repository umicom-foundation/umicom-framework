/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_risk_assessment.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate change, build, test and repository evidence into a reviewable engineering risk score.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/risk_assessment.h"
int main(void){ UmiHelixRiskAssessment r; umi_helix_risk_assessment_init(&r,"risk_assessment"); if(umi_helix_risk_assessment_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_risk_assessment_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
