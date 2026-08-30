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

size_t umi_test_platform_build_readiness_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_at(size_t index)
{
    return index < umi_test_platform_build_readiness_command_count()
        ? &COMMANDS[index] : NULL;
}

const UmiTestPlatformBuildReadinessCommand *
umi_test_platform_build_readiness_command_find(const char *id)
{
    size_t index;
    if (id == NULL) return NULL;
    for (index = 0U; index <
        umi_test_platform_build_readiness_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}
