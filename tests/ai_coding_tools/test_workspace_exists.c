/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_workspace_exists.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools workspace exists.
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
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage, "CMakeLists.txt", "project(Test)\n") ==
        UMI_STATUS_OK);

    call.call_id = 2U;
    (void)strcpy(call.tool_id, "workspace.exists");
    (void)strcpy(call.arguments_json, "{\"path\":\"CMakeLists.txt\"}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"exists\":true") != NULL);

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
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &state->f.workspace_storage, "CMakeLists.txt", "project(Test)\n") ==
        UMI_STATUS_OK);

    call.call_id = 2U;
    (void)strcpy(call.tool_id, "workspace.exists");
    (void)strcpy(call.arguments_json, "{\"path\":\"CMakeLists.txt\"}");

    assert(umi_ai_coding_tool_execute(
        &state->f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"exists\":true") != NULL);

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
