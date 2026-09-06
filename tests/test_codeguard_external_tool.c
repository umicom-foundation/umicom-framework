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

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/codeguard.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    char b[256];
    /* A normal path is quoted so a workspace directory containing spaces remains one argument. */
    assert(umi_codeguard_external_tool_command(
        UMI_CODEGUARD_TOOL_CLANG_ANALYZER,"folder with spaces/x.c",b,sizeof(b)));
    assert(strstr(b,"--analyze")!=NULL);
    assert(strstr(b,"\"folder with spaces/x.c\"")!=NULL);
    /* Shell metacharacters are rejected instead of being copied into executable command text. */
    assert(!umi_codeguard_external_tool_command(
        UMI_CODEGUARD_TOOL_CLANG_ANALYZER,"x.c; erase important.txt",b,sizeof(b)));
    /* Windows environment expansion is rejected as well as command separators. */
    assert(!umi_codeguard_external_tool_command(
        UMI_CODEGUARD_TOOL_CLANG_ANALYZER,"%TEMP%/x.c",b,sizeof(b)));
    return 0;
}
