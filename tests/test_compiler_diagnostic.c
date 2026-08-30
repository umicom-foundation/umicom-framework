/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_diagnostic.c
 *
 * PURPOSE:
 *   Implement the test compiler diagnostic behavior for
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
int main(void) { UmiCompilerDiagnostic diagnostic; UmiCompilerDiagnosticSet set = {0}; assert(umi_compiler_diagnostic_parse_line("main.c:12:4: error: expected expression",&diagnostic) == UMI_STATUS_OK); assert(diagnostic.line == 12U); assert(diagnostic.severity == UMI_COMPILER_DIAGNOSTIC_ERROR); assert(umi_compiler_diagnostic_set_add(&set,&diagnostic) == UMI_STATUS_OK); assert(set.errors == 1U); return 0; }
