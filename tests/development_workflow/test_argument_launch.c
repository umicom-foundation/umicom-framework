/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_argument_launch.c
 * PURPOSE: Check reviewed vectors through the real native launch composition and a controlled DAP peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/arguments.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); result = 1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int result = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    char directory[UMI_PATH_CAPACITY];
    CHECK(umi_fs_temp_directory(directory, sizeof(directory)) == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    /* This peer verifies the DAP args array; it never runs a debuggee. */
    const char *values[] = {"--umicom-argument-check", "two words", "", "C:\\Folder with spaces\\", "caf\xc3\xa9", "literal;$(value)"};
    char oversized[UMI_ARGUMENT_TEXT_CAPACITY];
    memset(oversized, '\\', sizeof(oversized) - 1U); oversized[sizeof(oversized) - 1U] = '\0';
    const char *excessive[] = {oversized};
    CHECK(UmiDebugRuntimePlatformLaunchArguments(platform, "lldb", argv[1], argv[1], directory,
        excessive, 1U, 2000U) == UMI_STATUS_CAPACITY_EXCEEDED);
    UmiDebugService *service = umi_debug_runtime_platform_service(platform);
    CHECK(umi_debug_launch_configuration_registry_count(umi_debug_service_launch_configuration(service)) == 0U);
    CHECK(UmiDebugRuntimePlatformLaunchArguments(platform, "lldb", argv[1], argv[1], directory,
        values, 6U, 2000U) == UMI_STATUS_OK);
    UmiDebugLaunchConfigurationSnapshot configuration;
    CHECK(umi_debug_launch_configuration_registry_find(umi_debug_service_launch_configuration(service),
        "native.launch", &configuration) == UMI_STATUS_OK);
    UmiArguments parsed;
    CHECK(UmiArgumentsParse(configuration.arguments, &parsed) == UMI_STATUS_OK && parsed.count == 6U);
    for (size_t index = 0U; index < parsed.count; ++index) CHECK(strcmp(parsed.values[index], values[index]) == 0);
cleanup:
    umi_debug_runtime_platform_destroy(platform);
    return result;
}
