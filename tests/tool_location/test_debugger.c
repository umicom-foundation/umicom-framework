/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/test_debugger.c
 * PURPOSE: Exercise project-scoped native debugger launch through a controlled DAP child.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/platform.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #value);                                         \
            result = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
static int ProgramMain(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    int result = 0;
    const char *mode = argv[1], *folder = argv[2], *peer = argv[3];
    UmiDebugRuntimePlatform *platform = NULL;
    char *before = NULL, *after = NULL;
    CHECK(UmiProcessSearchPathCapture(folder, &before) == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    const char *kind = strcmp(mode, "gdb") == 0 ? "gdb" : "lldb";
    const char *adapter = strcmp(mode, "absolute") == 0 ? peer : NULL;
    const char *selected = folder;
    if (strcmp(mode, "invalid-folder") == 0)
        selected = "relative/tools";
    if (strcmp(mode, "relative-adapter") == 0)
        adapter = "relative/adapter";
    char missing[UMI_PATH_CAPACITY];
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(umi_path_join(folder, "no-adapter-here", missing, sizeof missing) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(missing));
        selected = missing;
    }
    const char *arguments[] = {"--umicom-tool-path-check", folder};
    UmiStatus status = UmiDebugRuntimePlatformLaunchArgumentsWithToolDirectory(
        platform, kind, adapter, peer, folder, arguments, 2U, 3000U, selected);
    UmiDebugRuntimePlatformSnapshot snapshot;
    CHECK(umi_debug_runtime_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid-folder") == 0 || strcmp(mode, "relative-adapter") == 0)
    {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !snapshot.active);
        UmiDebugService *service = umi_debug_runtime_platform_service(platform);
        CHECK(umi_debug_launch_configuration_registry_count(
                  umi_debug_service_launch_configuration(service)) == 0U);
    }
    else if (strcmp(mode, "missing") == 0)
        CHECK(status == UMI_STATUS_UNAVAILABLE && !snapshot.active);
    else
        CHECK(status == UMI_STATUS_OK && snapshot.active && snapshot.initialized);
    CHECK(UmiProcessSearchPathCapture(folder, &after) == UMI_STATUS_OK);
    CHECK(strcmp(before, after) == 0);
cleanup:
    umi_debug_runtime_platform_destroy(platform);
    UmiProcessSearchPathFree(before);
    UmiProcessSearchPathFree(after);
    return result;
}
#include "../native_process/utf8_entry.inc"
