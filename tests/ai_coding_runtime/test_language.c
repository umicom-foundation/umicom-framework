/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_language.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime language contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/language.h"

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
