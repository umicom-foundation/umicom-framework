/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_semantic_issue.c
 *
 * PURPOSE:
 *   Regression coverage for capture compiler semantic diagnostics with stable codes, severity, spans and fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_issue.h"
int main(void){UmiNativeSemanticIssue i;UmiNativeSourceSpan s={0};if(umi_nc_semantic_issue_init(&i,UMI_NC_SEM_ERROR,1001U,s,"undeclared identifier")!=UMI_STATUS_OK)return 1;if(!umi_nc_semantic_issue_blocks_codegen(&i)||i.fingerprint==0U)return 2;return 0;}
