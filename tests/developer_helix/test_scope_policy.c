/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_scope_policy.c
 *
 * PURPOSE:
 *   Regression coverage for constrain autonomous engineering work to explicitly permitted repository, path and operation scope.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/scope_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixScopePolicy r; umi_helix_scope_policy_init(&r,"scope_policy"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_scope_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_scope_policy_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
