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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
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


#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    ToolTestFixture f;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    char prompt[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];

    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_prompt_build(
        &state->f.environment, prompt, sizeof(prompt)) == UMI_STATUS_OK);
    assert(strstr(prompt, "workspace.read") != NULL);
    assert(strstr(prompt, "language.symbols") != NULL);
    assert(strstr(prompt, "developer.build") == NULL);
    assert(strstr(prompt, "source-control.push") == NULL);

    tool_test_fixture_deinit(&state->f);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
