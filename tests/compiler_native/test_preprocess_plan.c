/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_preprocess_plan.c
 *
 * PURPOSE:
 *   Regression coverage for scan preprocessor structure, count directives and validate conditional nesting before expansion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/preprocess_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiNativeSourceBuffer s; umi_nc_source_buffer_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_source_buffer_set(&s,"x.c","#define A 1\n#if A\n#include <x.h>\n#endif\n")!=UMI_STATUS_OK) return 1; UmiNativePreprocessPlan p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_preprocess_plan_scan(&s,&p)!=UMI_STATUS_OK) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(p.defines!=1U||p.includes!=1U||p.max_conditional_depth!=1U||!umi_nc_preprocess_plan_ready(&p)) return 3; return 0; }
