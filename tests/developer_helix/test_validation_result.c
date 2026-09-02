/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_validation_result.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate gate outcomes into a deterministic validation decision and blocker count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_result.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixValidationResult r; umi_helix_validation_result_init(&r); umi_helix_validation_result_record(&r,true,true); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_validation_result_finalize(&r)!=UMI_HELIX_DECISION_ALLOW||r.score!=1.0){ return 1; } umi_helix_validation_result_record(&r,false,true); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_validation_result_finalize(&r)!=UMI_HELIX_DECISION_REVIEW||r.blocked!=1U){ return 2; } return 0; }
