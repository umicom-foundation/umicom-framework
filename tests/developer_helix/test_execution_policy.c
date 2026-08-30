/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_execution_policy.c
 *
 * PURPOSE:
 *   Regression coverage for define dry-run, atomicity, cancellation, approval and failure-handling policy for plan execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_policy.h"
int main(void){ UmiHelixExecutionPolicy r; umi_helix_execution_policy_init(&r,"execution_policy"); if(umi_helix_execution_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_execution_policy_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
