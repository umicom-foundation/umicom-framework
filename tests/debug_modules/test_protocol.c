/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_modules/test_protocol.c
 * PURPOSE: Exercise native module paging and stale connection rejection through an inert adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/module_page.h"
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
/* The fixture may deliver its initial stop after the attach response. Drain
 * that one event before asserting inspection, without running a real target. */
static int ObserveFixtureEvent(UmiDebugRuntimePlatform *platform)
{
    int handled = 0;
    UmiStatus status = umi_debug_runtime_platform_pump_event(platform, 250U, &handled);
    return status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT;
}
int main(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"normal", "unknown",     "stale",         "session",
                           "refuse", "timeout",     "malformed",     "unsupported",
                           "closed", "new-session", "inflight-exit", "legacy-refresh"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugModulePage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY], fixture[32];
    char *log = NULL;
    size_t bytes = 0U;
    CHECK(page != NULL && before != NULL);
    memset(page, 0xA5, sizeof *page);
    *before = *page;
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    snprintf(fixture, sizeof fixture, "modules-%s", mode);
    CHECK(umi_fs_write_text(settings, strcmp(mode, "unsupported") == 0 ? "normal" : fixture) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
    CHECK(ObserveFixtureEvent(platform));
    UmiDebugModuleSession session;
    CHECK(UmiDebugRuntimeModuleSessionRead(platform, &session) == UMI_STATUS_OK);
    if (strcmp(mode, "legacy-refresh") == 0)
    {
        CHECK(umi_debug_runtime_platform_refresh_modules(platform, 2000U) == UMI_STATUS_OK);
        goto done;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "stale") == 0)
    {
        --session.generation;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "session") == 0)
    {
        strcpy(session.session_id, "old");
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    if (strcmp(mode, "inflight-exit") == 0)
        expected = UMI_STATUS_BUSY;
    if (strcmp(mode, "malformed") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "unsupported") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    CHECK(UmiDebugRuntimeModulePageRead(platform, &session, 0U, 32U,
                                        strcmp(mode, "timeout") == 0 ? 100U : 2000U,
                                        page) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(page, before, sizeof *page) == 0);
    else
    {
        CHECK(page->count == 32U && page->first == 0U && page->has_more &&
              page->items[0].numeric_id);
        CHECK(page->total_known == (strcmp(mode, "unknown") != 0));
        CHECK(UmiDebugRuntimeModulePageRead(platform, &session, 32U, 32U, 2000U, page) ==
              UMI_STATUS_OK);
        CHECK(page->count == 3U && page->items[0].number == 32 && !page->has_more);
    }
    if (strcmp(mode, "closed") != 0)
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    bool sent = expected != UMI_STATUS_INVALID_STATE && expected != UMI_STATUS_NOT_IMPLEMENTED &&
                expected != UMI_STATUS_NOT_FOUND;
    CHECK((strstr(log, "\"command\":\"modules\"") != NULL) == sent);
    if (strcmp(mode, "normal") == 0)
        CHECK(strstr(log, "\"startModule\":32") != NULL);
    if (strcmp(mode, "new-session") == 0)
    {
        CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
        CHECK(ObserveFixtureEvent(platform));
        CHECK(UmiDebugRuntimeModulePageRead(platform, &session, 0U, 32U, 100U, page) ==
              UMI_STATUS_INVALID_STATE);
    }
done:
    umi_debug_runtime_platform_destroy(platform);
    free(page);
    free(before);
    umi_fs_free_text(log);
    return failed;
}
