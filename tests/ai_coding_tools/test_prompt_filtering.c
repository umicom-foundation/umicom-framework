/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_prompt_filtering.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools prompt filtering.
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
#include "tool_test_support.h"

int main(void)
{
    ToolTestFixture f;
    char prompt[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_prompt_build(
        &f.environment, prompt, sizeof(prompt)) == UMI_STATUS_OK);
    assert(strstr(prompt, "workspace.read") != NULL);
    assert(strstr(prompt, "language.symbols") != NULL);
    assert(strstr(prompt, "developer.build") == NULL);
    assert(strstr(prompt, "source-control.push") == NULL);

    tool_test_fixture_deinit(&f);
    return 0;
}

