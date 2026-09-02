/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/command.c
 * PURPOSE: Implement stable build-readiness command discovery.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/command.h"

#include <string.h>

static const UmiTestPlatformBuildReadinessCommand COMMANDS[] = {
    {"test.configure.fresh", "Configure Fresh", "Test Readiness", true},
    {"test.select.product-preset", "Select Product Preset", "Test Readiness", true},
    {"test.build.required-artifacts", "Build Required Test Artifacts", "Test Readiness", true},
    {"test.inspect.registration", "Inspect Test Registration", "Test Readiness", false},
    {"test.inspect.missing-artifacts", "Inspect Missing Test Artifacts", "Test Readiness", false},
    {"test.explain.not-run", "Explain Not Run", "Test Readiness", false},
    {"test.run.focused", "Run Focused Tests", "Testing", true},
    {"test.run.complete", "Run Complete Test Suite", "Testing", true},
    {"test.export.readiness", "Export Readiness Report", "Test Readiness", false}
};

/*
 * Return the number of records represented by test platform build readiness command
 * without changing their state.
 */
size_t umi_test_platform_build_readiness_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find test platform build readiness command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_at(size_t index)
{
    return index < umi_test_platform_build_readiness_command_count()
        ? &COMMANDS[index] : NULL;
}

/*
 * Find test platform build readiness command while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_find(const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index <
        umi_test_platform_build_readiness_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}
