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

