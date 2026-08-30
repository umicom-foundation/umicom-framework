/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_configuration.c
 *
 * PURPOSE:
 *   Verify workbench configuration validation and BuildRequest projection.
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

#include "umicom/developer_workbench/configuration.h"

int main(void)
{
    UmiDeveloperWorkbenchConfiguration configuration;
    UmiBuildRequest request;

    umi_developer_workbench_configuration_init(
        &configuration, "debug", "Debug");
    (void)strcpy(configuration.source_root, "C:/work/project");
    (void)strcpy(configuration.build_directory, "build/debug");
    (void)strcpy(configuration.preset, "windows-ucrt64-debug");
    (void)strcpy(configuration.executable, "build/debug/bin/app.exe");

    assert(umi_developer_workbench_configuration_validate(
        &configuration, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_developer_workbench_configuration_to_build_request(
        &configuration, &request) == UMI_STATUS_OK);
    assert(strcmp(request.preset, "windows-ucrt64-debug") == 0);
    assert(strcmp(request.executable, "build/debug/bin/app.exe") == 0);
    return 0;
}
