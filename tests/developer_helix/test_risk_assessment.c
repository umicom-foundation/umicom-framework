/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_risk_assessment.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate change, build, test and repository evidence into a reviewable engineering risk score.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/risk_assessment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRiskAssessment r; umi_helix_risk_assessment_init(&r,"risk_assessment"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_risk_assessment_validate(&r)!=UMI_STATUS_OK){ return 1; } r.risk=0.2; double s=umi_helix_risk_assessment_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
