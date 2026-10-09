/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exceptions/test_protocol.c
 * PURPOSE: Exercise exception defaults, stale choices and uncertain replies through an inert DAP adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/exception_filters.h"
#include "umicom/debug_runtime/native_attach.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    const char *mode = argv[1];
    const char *modes[] = {"normal", "clear",   "stale",     "session",    "count",       "boolean",
                           "refuse", "timeout", "malformed", "unverified", "unsupported", "closed"};
    bool known = false;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(mode, modes[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY], fixture[32];
    char *log = NULL;
    size_t bytes = 0U;
    UmiDebugRuntimePlatform *platform = NULL;
    /* Retain the prior declaration for review. The protocol checks still cover
     * the same session state, now through the distinct filter aggregate type. */
#if 0
    UmiDebugExceptionSnapshot *snapshot = calloc(1U, sizeof *snapshot);
#endif
    UmiDebugExceptionFiltersSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    CHECK(snapshot != NULL);
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    snprintf(fixture, sizeof fixture, "exceptions-%s", mode);
    CHECK(umi_fs_write_text(settings, strcmp(mode, "unsupported") == 0 ? "normal" : fixture) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
    CHECK(UmiDebugRuntimeExceptionFiltersRead(platform, snapshot) == UMI_STATUS_OK);
    if (strcmp(mode, "unsupported") == 0)
    {
        CHECK(snapshot->catalog.count == 0U);
        CHECK(UmiDebugRuntimeExceptionFiltersApply(platform, &snapshot->selection, 100U) ==
              UMI_STATUS_NOT_IMPLEMENTED);
    }
    else
    {
        CHECK(snapshot->catalog.count == 2U && snapshot->acknowledged);
        CHECK(snapshot->selection.enabled[0] == 1 && snapshot->selection.enabled[1] == 0);
        CHECK(snapshot->acknowledgement.verified[0] == -1);
        UmiDebugExceptionSelection selection = snapshot->selection;
        selection.enabled[0] = 0;
        selection.enabled[1] = strcmp(mode, "clear") != 0;
        UmiStatus expected = UMI_STATUS_OK;
        if (strcmp(mode, "stale") == 0)
        {
            --selection.revision;
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "session") == 0)
        {
            strcpy(selection.session_id, "another-session");
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "count") == 0)
        {
            selection.count = 1U;
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
        if (strcmp(mode, "boolean") == 0)
        {
            selection.enabled[1] = 2;
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
        if (strcmp(mode, "refuse") == 0)
            expected = UMI_STATUS_UNAVAILABLE;
        if (strcmp(mode, "timeout") == 0)
            expected = UMI_STATUS_TIMEOUT;
        if (strcmp(mode, "malformed") == 0)
            expected = UMI_STATUS_PARSE_ERROR;
        if (strcmp(mode, "closed") == 0)
        {
            CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
            expected = UMI_STATUS_NOT_FOUND;
        }
        CHECK(UmiDebugRuntimeExceptionFiltersApply(
                  platform, &selection, strcmp(mode, "timeout") == 0 ? 100U : 2000U) == expected);
        if (strcmp(mode, "closed") != 0)
        {
            uint64_t previous = snapshot->selection.revision;
            CHECK(UmiDebugRuntimeExceptionFiltersRead(platform, snapshot) == UMI_STATUS_OK);
            bool preflight =
                expected == UMI_STATUS_INVALID_STATE || expected == UMI_STATUS_INVALID_ARGUMENT;
            CHECK(preflight ? snapshot->selection.revision == previous
                            : snapshot->selection.revision > previous);
            if (!preflight)
                CHECK(snapshot->acknowledged == (expected == UMI_STATUS_OK));
            if (strcmp(mode, "unverified") == 0)
                CHECK(snapshot->acknowledgement.verified[0] == 0);
        }
    }
    if (strcmp(mode, "closed") != 0)
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    const char *configure = strstr(log, "\"command\":\"setExceptionBreakpoints\"");
    if (strcmp(mode, "unsupported") == 0)
        CHECK(configure == NULL);
    else
    {
        const char *complete = strstr(log, "\"command\":\"configurationDone\"");
        CHECK(configure != NULL && complete != NULL && complete > configure);
        CHECK(strstr(configure, "\"filters\":[\"caught\"]") != NULL);
        if (strcmp(mode, "normal") == 0)
            CHECK(strstr(log, "\"filters\":[\"uncaught\"]") != NULL);
        if (strcmp(mode, "clear") == 0)
            CHECK(strstr(log, "\"filters\":[]") != NULL);
    }
done:
    umi_debug_runtime_platform_destroy(platform);
    free(snapshot);
    umi_fs_free_text(log);
    return failed;
}
