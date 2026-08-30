/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_readiness.c
 *
 * PURPOSE:
 *   Verify incomplete toolchains disable executable lifecycle actions.
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

#include "umicom/developer_workbench/readiness.h"

int main(void)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiToolchainProfile profile;
    UmiDeveloperWorkbenchReadiness readiness;

    umi_developer_workbench_configuration_init(
        &configuration, "debug", "Debug");
    (void)strcpy(configuration.source_root, "C:/work/project");
    (void)strcpy(configuration.build_directory, "build/debug");

    umi_toolchain_profile_init(&profile);

    assert(umi_developer_workbench_readiness_evaluate(
        &configuration, &profile, &readiness) == UMI_STATUS_OK);
    assert(readiness.configuration_valid == 1);
    assert(readiness.toolchain_ready == 0);
    assert(readiness.can_build == 0);
    return 0;
}
