/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_rollback_plan.c
 *
 * PURPOSE:
 *   Regression coverage for describe reverse-order restoration steps and the conditions that make rollback mandatory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/rollback_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixRollbackPlan r; umi_helix_rollback_plan_init(&r,"rollback_plan"); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_helix_rollback_plan_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_rollback_plan_score(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s<=0.0 || s>1.0){ return 2; } return 0; }
