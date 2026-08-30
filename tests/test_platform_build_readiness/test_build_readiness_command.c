/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_platform_build_readiness/test_build_readiness_command.c
 * PURPOSE: Focused regression for the Framework build-readiness platform.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/test_platform/build_readiness/command.h"
int main(void) {
    size_t index;
    assert(umi_test_platform_build_readiness_command_count() == 9U);
    assert(umi_test_platform_build_readiness_command_find(
        "test.explain.not-run") != NULL);
    for (index = 0U; index <
        umi_test_platform_build_readiness_command_count(); ++index) {
        const UmiTestPlatformBuildReadinessCommand *command =
            umi_test_platform_build_readiness_command_at(index);
        assert(command != NULL && strncmp(command->id, "test.", 5U) == 0);
    }
    assert(umi_test_platform_build_readiness_command_at(99U) == NULL);
    return 0;
}

