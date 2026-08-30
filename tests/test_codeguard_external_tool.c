/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_external_tool.c
 *
 * PURPOSE:
 *   Validate a focused CodeGuard contract so security tooling remains
 *   deterministic during future refactors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/codeguard.h"
int main(void){char b[256];assert(umi_codeguard_external_tool_command(UMI_CODEGUARD_TOOL_CLANG_ANALYZER,"x.c",b,sizeof(b)));assert(strstr(b,"--analyze")!=NULL);return 0;}
