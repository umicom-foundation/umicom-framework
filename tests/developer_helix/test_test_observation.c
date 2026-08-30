/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_test_observation.c
 *
 * PURPOSE:
 *   Regression coverage for capture test outcome, pass/fail/skip counts and regression evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/test_observation.h"
int main(void){ UmiHelixTestObservation r; umi_helix_test_observation_init(&r,"test_observation"); if(umi_helix_test_observation_validate(&r)!=UMI_STATUS_OK){ return 1; } r.passed=9U; r.failed=1U; double s=umi_helix_test_observation_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
