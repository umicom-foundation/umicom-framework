/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_semantic_issue.c
 *
 * PURPOSE:
 *   Regression coverage for capture compiler semantic diagnostics with stable codes, severity, spans and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_issue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeSemanticIssue i;UmiNativeSourceSpan s={0};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_semantic_issue_init(&i,UMI_NC_SEM_ERROR,1001U,s,"undeclared identifier")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_semantic_issue_blocks_codegen(&i)||i.fingerprint==0U)return 2;return 0;}
