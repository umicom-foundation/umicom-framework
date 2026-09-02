/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_language.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime language contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/language.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    assert(strcmp(
        umi_ai_coding_runtime_language_for_path("src/main.c"), "c") == 0);
    assert(strcmp(
        umi_ai_coding_runtime_language_for_path("CMakeLists.txt"), "cmake") == 0);
    assert(strcmp(
        umi_ai_coding_runtime_language_for_path("src/app.ts"), "typescript") == 0);
    assert(umi_ai_coding_runtime_path_is_text_source("README.md"));
    assert(!umi_ai_coding_runtime_path_is_text_source("image.png"));

    return 0;
}
