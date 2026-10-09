/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/test_debugger.c
 * PURPOSE: Check native debugger environment delivery and rejection before session publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
int main(int argc, char **argv)
{
    CHECK(argc == 4);
    UmiDebugRuntimePlatform *platform = NULL;
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    const char *arguments[] = {"--umicom-environment-check"};
    const char *definitions = strcmp(argv[1], "invalid") == 0
                                  ? "MODE=one mode=two"
                                  : "UMICOM_RUN_VALUE='debugger inherits selected'";
    UmiStatus status = UmiDebugRuntimePlatformLaunchArgumentsWithEnvironment(
        platform, strcmp(argv[1], "gdb") == 0 ? "gdb" : "lldb", argv[3], argv[3], argv[2],
        arguments, 1U, 3000U, argv[2], definitions);
    UmiDebugRuntimePlatformSnapshot snapshot;
    CHECK(umi_debug_runtime_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    UmiDebugService *service = umi_debug_runtime_platform_service(platform);
    if (strcmp(argv[1], "invalid") == 0)
    {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !snapshot.active);
        CHECK(umi_debug_launch_configuration_registry_count(
                  umi_debug_service_launch_configuration(service)) == 0U);
    }
    else
    {
        CHECK(status == UMI_STATUS_OK && snapshot.active && snapshot.initialized);
        UmiDebugLaunchConfigurationSnapshot configuration;
        CHECK(umi_debug_launch_configuration_registry_find(
                  umi_debug_service_launch_configuration(service), "native.launch",
                  &configuration) == UMI_STATUS_OK);
        CHECK(strcmp(configuration.environment, definitions) == 0);
    }
    umi_debug_runtime_platform_destroy(platform);
    return 0;
}
