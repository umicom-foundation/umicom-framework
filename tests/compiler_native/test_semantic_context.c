/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_semantic_context.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate semantic diagnostics and expose a single code-generation readiness decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_context.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeSemanticContext c;umi_nc_semantic_context_init(&c);UmiNativeSemanticIssue i;UmiNativeSourceSpan s={0};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_semantic_issue_init(&i,UMI_NC_SEM_WARNING,1U,s,"conversion")!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_semantic_context_report(&c,&i)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_semantic_context_can_codegen(&c)||c.warnings!=1U)return 3;return 0;}
