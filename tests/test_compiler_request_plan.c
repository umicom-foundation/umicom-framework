/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_request_plan.c
 *
 * PURPOSE:
 *   Implement the test compiler request plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiCompilerRequest request; UmiCompilerPlan plan; UmiCompilerProvider clang = umi_compiler_clang_provider("clang","x86_64-unknown-linux-gnu"); assert(umi_compiler_request_init(&request,"demo","main",UMI_COMPILER_LANGUAGE_C,"main.c","main.o") == UMI_STATUS_OK); assert(umi_compiler_request_add_definition(&request,"DEMO=1") == UMI_STATUS_OK); assert(umi_compiler_provider_plan(&clang,&request,&plan) == UMI_STATUS_OK); assert(strcmp(plan.provider_id,"clang") == 0); assert(plan.argument_count >= 6U); return 0; }
