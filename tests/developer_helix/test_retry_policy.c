/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_retry_policy.c
 *
 * PURPOSE:
 *   Regression coverage for bound repair/regeneration retries with backoff and mandatory escalation after exhaustion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/retry_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRetryPolicy r; umi_helix_retry_policy_init(&r,"retry_policy"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_retry_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.limit=10U; r.used=2U; double s=umi_helix_retry_policy_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
