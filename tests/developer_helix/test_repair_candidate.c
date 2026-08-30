/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_repair_candidate.c
 *
 * PURPOSE:
 *   Regression coverage for represent one evidence-backed repair proposal with confidence, risk and estimated change size.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/repair_candidate.h"
int main(void){ UmiHelixRepairCandidate r; umi_helix_repair_candidate_init(&r,"repair_candidate"); if(umi_helix_repair_candidate_validate(&r)!=UMI_STATUS_OK){ return 1; } r.confidence=0.9; r.risk=0.1; r.cost=0.2; double s=umi_helix_repair_candidate_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
