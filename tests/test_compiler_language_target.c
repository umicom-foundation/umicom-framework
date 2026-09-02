/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_language_target.c
 *
 * PURPOSE:
 *   Implement the test compiler language target behavior for
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
int main(void) { UmiCompilerTarget target; assert(umi_compiler_language_from_extension("engine.cpp") == UMI_COMPILER_LANGUAGE_CPP); assert(strcmp(umi_compiler_language_default_standard(UMI_COMPILER_LANGUAGE_C),"c23") == 0); assert(umi_compiler_target_parse("x86_64-pc-windows-gnu",&target) == UMI_STATUS_OK); assert(target.object_format == UMI_COMPILER_OBJECT_COFF); assert(target.pointer_width == 64U); return 0; }
