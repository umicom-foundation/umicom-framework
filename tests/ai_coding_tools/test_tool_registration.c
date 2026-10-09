/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_tool_registration.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools tool registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
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

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(f.runtime.tools.count == umi_ai_coding_tool_catalogue_count());
    assert(umi_ai_tool_registry_find(
        &f.runtime.tools, "workspace.read") != NULL);
    assert(umi_ai_tool_registry_find(
        &f.runtime.tools, "developer.build") != NULL);
    assert(umi_ai_tool_registry_find(
        &f.runtime.tools, "agent.apply") != NULL);

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

    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);
    assert(state->f.runtime.tools.count == umi_ai_coding_tool_catalogue_count());
    assert(umi_ai_tool_registry_find(
        &state->f.runtime.tools, "workspace.read") != NULL);
    assert(umi_ai_tool_registry_find(
        &state->f.runtime.tools, "developer.build") != NULL);
    assert(umi_ai_tool_registry_find(
        &state->f.runtime.tools, "agent.apply") != NULL);

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
