/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/function_breakpoints/test_protocol.c
 * PURPOSE: Exercise function-breakpoint capability gates and exact-session publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/function_breakpoint_session.h"
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
    const char *modes[] = {"normal",     "clear",       "disabled",    "conditions", "no-condition",
                           "stale",      "session",     "refuse",      "timeout",    "malformed",
                           "unverified", "unsupported", "new-session", "closed"};
    bool known = false;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(mode, modes[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugFunctionSnapshot *snapshot = calloc(1U, sizeof *snapshot);
    UmiDebugFunctionDraft *draft = calloc(1U, sizeof *draft);
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY], fixture[32];
    char *log = NULL;
    size_t bytes = 0U;
    CHECK(snapshot != NULL && draft != NULL);
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    snprintf(fixture, sizeof fixture, "functions-%s", mode);
    CHECK(umi_fs_write_text(settings, strcmp(mode, "unsupported") == 0 ? "normal" : fixture) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
    CHECK(UmiDebugRuntimeFunctionBreakpointsRead(platform, snapshot) == UMI_STATUS_OK);
    CHECK(snapshot->draft.count == 0U && !snapshot->acknowledged);
    *draft = snapshot->draft;
    draft->count = strcmp(mode, "clear") == 0 ? 0U : 1U;
    strcpy(draft->entries[0].breakpoint.name, "main");
    draft->entries[0].enabled = strcmp(mode, "disabled") != 0;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "conditions") == 0 || strcmp(mode, "no-condition") == 0)
    {
        strcpy(draft->entries[0].breakpoint.condition, "counter > 3");
        strcpy(draft->entries[0].breakpoint.hit_condition, "5");
    }
    if (strcmp(mode, "no-condition") == 0 || strcmp(mode, "unsupported") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "stale") == 0)
    {
        --draft->revision;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "session") == 0)
    {
        strcpy(draft->session_id, "old-session");
        expected = UMI_STATUS_INVALID_STATE;
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
    CHECK(UmiDebugRuntimeFunctionBreakpointsApply(
              platform, draft, strcmp(mode, "timeout") == 0 ? 100U : 2000U) == expected);
    bool sent = expected != UMI_STATUS_NOT_IMPLEMENTED && expected != UMI_STATUS_INVALID_STATE &&
                expected != UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "closed") != 0)
    {
        uint64_t before = snapshot->draft.revision;
        CHECK(UmiDebugRuntimeFunctionBreakpointsRead(platform, snapshot) == UMI_STATUS_OK);
        CHECK(sent ? snapshot->draft.revision > before : snapshot->draft.revision == before);
        CHECK(snapshot->acknowledged == (expected == UMI_STATUS_OK));
        if (strcmp(mode, "unverified") == 0)
            CHECK(!snapshot->reply.entries[0].verified);
        if (strcmp(mode, "normal") == 0)
            CHECK(snapshot->reply.entries[0].verified &&
                  snapshot->reply.entries[0].adapter_id == 100U);
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
    }
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    CHECK((strstr(log, "\"command\":\"setFunctionBreakpoints\"") != NULL) == sent);
    if (strcmp(mode, "clear") == 0 || strcmp(mode, "disabled") == 0)
        CHECK(strstr(log, "\"breakpoints\":[]") != NULL);
    if (strcmp(mode, "conditions") == 0)
        CHECK(strstr(log, "\"condition\":\"counter > 3\"") != NULL &&
              strstr(log, "\"hitCondition\":\"5\"") != NULL);
    if (strcmp(mode, "new-session") == 0)
    {
        CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
        CHECK(UmiDebugRuntimeFunctionBreakpointsRead(platform, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->draft.count == 0U && !snapshot->acknowledged);
        CHECK(UmiDebugRuntimeFunctionBreakpointsApply(platform, draft, 100U) ==
              UMI_STATUS_INVALID_STATE);
    }
done:
    umi_debug_runtime_platform_destroy(platform);
    free(snapshot);
    free(draft);
    umi_fs_free_text(log);
    return failed;
}
