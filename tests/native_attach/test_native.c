/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/test_native.c
 * PURPOSE: Exercise native DAP attachment sequencing and teardown using an inert adapter process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/native_attach.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    int failed = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    char *log = NULL;
    size_t bytes = 0U;
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(settings, argv[1]) == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {
        strcmp(argv[1], "lldb") == 0 ? "lldb" : "gdb", argv[2], "", argv[3], "", 123456U};
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(argv[1], "self") == 0)
    {
#ifdef _WIN32
        options.process_id = (uint64_t)GetCurrentProcessId();
#else
        options.process_id = (uint64_t)getpid();
#endif
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(argv[1], "refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    else if (strcmp(argv[1], "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    else
        CHECK(strcmp(argv[1], "normal") == 0 || strcmp(argv[1], "lldb") == 0 ||
              strcmp(argv[1], "deferred") == 0 || strcmp(argv[1], "destroy") == 0 ||
              strcmp(argv[1], "busy") == 0);
    CHECK(UmiDebugRuntimePlatformAttachNative(
              platform, &options, strcmp(argv[1], "timeout") == 0 ? 200U : 5000U) == expected);
    UmiDebugRuntimePlatformSnapshot snapshot;
    CHECK(umi_debug_runtime_platform_snapshot(platform, &snapshot) == UMI_STATUS_OK);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(snapshot.active && snapshot.attached &&
              strcmp(snapshot.active_configuration_id, "native.attach") == 0);
        if (strcmp(argv[1], "busy") == 0)
            CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 100U) == UMI_STATUS_BUSY);
        if (strcmp(argv[1], "destroy") == 0)
        {
            umi_debug_runtime_platform_destroy(platform);
            platform = NULL;
        }
        else
            CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
    }
    else
        CHECK(!snapshot.active);
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    if (strcmp(argv[1], "self") == 0)
        CHECK(bytes == 0U);
    else
    {
        CHECK(strstr(log, "\"command\":\"attach\"") != NULL &&
              strstr(log, "\"pid\":123456") != NULL);
        CHECK(strstr(log, "\"command\":\"launch\"") == NULL);
        CHECK(strstr(log, "\"command\":\"disconnect\"") != NULL &&
              strstr(log, "\"terminateDebuggee\":false") != NULL);
        CHECK(strstr(log, "\"terminateDebuggee\":true") == NULL);
        if (expected == UMI_STATUS_OK)
            CHECK(strstr(log, "\"command\":\"configurationDone\"") != NULL);
    }
done:
    umi_debug_runtime_platform_destroy(platform);
    umi_fs_free_text(log);
    return failed;
}
