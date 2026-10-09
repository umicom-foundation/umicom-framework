/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exception_information/test_protocol.c
 * PURPOSE: Exercise exception-stop capability and revision guards with an inert recorded adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/exception_inspection.h"
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
    const char *mode = argv[1], *cases[] = {"normal",  "unsupported", "other-stop", "stale",
                                            "thread",  "connection",  "closed",     "refuse",
                                            "timeout", "malformed",   "continued"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugExceptionInformation *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY], fixture[40];
    char *log = NULL;
    size_t bytes = 0U;
    CHECK(out != NULL && before != NULL);
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    snprintf(fixture, sizeof fixture, "information-%s", mode);
    CHECK(umi_fs_write_text(settings, strcmp(mode, "unsupported") == 0 ? "normal" : fixture) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
    /* A real host pumps events continuously. The fixture's one initial stop
     * may arrive after attach returns, so observe it before capturing a target. */
    int handled = 0;
    UmiStatus observed = umi_debug_runtime_platform_pump_event(platform, 250U, &handled);
    CHECK(observed == UMI_STATUS_OK || observed == UMI_STATUS_TIMEOUT);
    UmiDebugExceptionTarget target;
    memset(&target, 0xA5, sizeof target);
    UmiDebugExceptionTarget previous = target;
    UmiStatus expected = strcmp(mode, "unsupported") == 0  ? UMI_STATUS_NOT_IMPLEMENTED
                         : strcmp(mode, "other-stop") == 0 ? UMI_STATUS_NOT_FOUND
                                                           : UMI_STATUS_OK;
    CHECK(UmiDebugRuntimeExceptionTargetRead(platform, &target) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(memcmp(&target, &previous, sizeof target) == 0);
        goto done;
    }
    CHECK(target.thread_id == 1U && target.connection.generation != 0U);
    if (strcmp(mode, "stale") == 0)
    {
        --target.revision;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "thread") == 0)
    {
        target.thread_id = 2U;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "connection") == 0)
    {
        --target.connection.generation;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    if (strcmp(mode, "malformed") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "continued") == 0)
        expected = UMI_STATUS_BUSY;
    CHECK(UmiDebugRuntimeExceptionInformationRead(
              platform, &target, strcmp(mode, "timeout") == 0 ? 100U : 2000U, out) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(out, before, sizeof *out) == 0);
    else
        CHECK(out->count == 2U && strcmp(out->details[0].evaluate_name, "dangerous()") == 0);
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    bool sent = expected != UMI_STATUS_INVALID_STATE && expected != UMI_STATUS_NOT_FOUND;
    CHECK((strstr(log, "\"command\":\"exceptionInfo\"") != NULL) == sent);
    CHECK(strstr(log, "\"command\":\"evaluate\"") == NULL &&
          strstr(log, "\"command\":\"continue\"") == NULL);
done:
    umi_debug_runtime_platform_destroy(platform);
    umi_fs_free_text(log);
    free(out);
    free(before);
    return failed;
}
