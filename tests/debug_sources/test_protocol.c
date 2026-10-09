/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_sources/test_protocol.c
 * PURPOSE: Exercise loaded-source reads and reference rejection through a recorded inert adapter connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/native_attach.h"
#include "umicom/debug_runtime/source_catalog.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <limits.h>
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
               *cases[] = {"normal",      "refuse",         "timeout",         "malformed",
                           "unsupported", "content-refuse", "content-timeout", "content-invalid",
                           "zero",        "large",          "stale",           "session",
                           "list-exit",   "content-exit",   "restart",         "closed",
                           "reconnect"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    int failed = 0;
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugSourceCatalog *catalog = malloc(sizeof *catalog), *before = malloc(sizeof *before);
    UmiDebugSourceContent *content = malloc(sizeof *content), *old = malloc(sizeof *old);
    char settings[UMI_PATH_CAPACITY], transcript[UMI_PATH_CAPACITY], fixture[40];
    char *log = NULL;
    size_t bytes = 0U;
    CHECK(catalog != NULL && before != NULL && content != NULL && old != NULL);
    memset(catalog, 0xA5, sizeof *catalog);
    *before = *catalog;
    memset(content, 0xA5, sizeof *content);
    *old = *content;
    CHECK(umi_path_join(argv[3], "attach-fixture-mode.txt", settings, sizeof settings) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(argv[3], "attach-fixture-requests.jsonl", transcript, sizeof transcript) ==
          UMI_STATUS_OK);
    snprintf(fixture, sizeof fixture, "sources-%s", mode);
    CHECK(umi_fs_write_text(settings, strcmp(mode, "unsupported") == 0 ? "normal" : fixture) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_write_text(transcript, "") == UMI_STATUS_OK);
    CHECK(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", argv[3], "", 123456U};
    CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
    CHECK(ObserveFixtureEvent(platform));
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    if (strcmp(mode, "list-exit") == 0)
        expected = UMI_STATUS_BUSY;
    if (strcmp(mode, "malformed") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "unsupported") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    CHECK(UmiDebugRuntimeSourceCatalogRead(platform, strcmp(mode, "timeout") == 0 ? 100U : 2000U,
                                           catalog) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(memcmp(catalog, before, sizeof *catalog) == 0);
        CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
        CHECK((strstr(log, "\"command\":\"loadedSources\"") != NULL) ==
              (strcmp(mode, "unsupported") != 0));
        CHECK(strstr(log, "\"command\":\"source\"") == NULL);
        goto done;
    }
    CHECK(catalog->count == 2U && catalog->items[0].reference == 42U &&
          catalog->items[1].reference == 0U);
    UmiDebugConnectionIdentity connection = catalog->connection;
    uint32_t reference = 42U;
    expected = UMI_STATUS_OK;
    if (strcmp(mode, "zero") == 0)
    {
        reference = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "large") == 0)
    {
        reference = (uint32_t)INT32_MAX + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "stale") == 0)
    {
        --connection.generation;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "session") == 0)
    {
        strcpy(connection.session_id, "old");
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "reconnect") == 0)
    {
        CHECK(umi_debug_runtime_platform_stop(platform, 0, 2000U) == UMI_STATUS_OK);
        CHECK(UmiDebugRuntimePlatformAttachNative(platform, &options, 5000U) == UMI_STATUS_OK);
        CHECK(ObserveFixtureEvent(platform));
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "restart") == 0)
    {
        CHECK(umi_debug_runtime_platform_restart(platform, 2000U) == UMI_STATUS_OK);
        CHECK(ObserveFixtureEvent(platform));
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "content-exit") == 0)
        expected = UMI_STATUS_BUSY;
    if (strcmp(mode, "content-refuse") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(mode, "content-timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    if (strcmp(mode, "content-invalid") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    CHECK(UmiDebugRuntimeSourceContentRead(platform, &connection, reference,
                                           strcmp(mode, "content-timeout") == 0 ? 100U : 2000U,
                                           content) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(content, old, sizeof *content) == 0);
    else
        CHECK(strcmp(content->text, "int generated(void) { return 42; }\n") == 0);
    CHECK(umi_fs_read_text(transcript, &log, &bytes) == UMI_STATUS_OK);
    bool requested = strcmp(mode, "normal") == 0 || strncmp(mode, "content-", 8U) == 0;
    CHECK((strstr(log, "\"command\":\"source\"") != NULL) == requested);
    if (requested)
    {
        CHECK(strstr(log, "\"source\":{\"sourceReference\":42},\"sourceReference\":42") != NULL);
        CHECK(strstr(log, "/untrusted/local.c") == NULL &&
              strstr(log, "/remote/generated.c") == NULL);
    }
done:
    umi_debug_runtime_platform_destroy(platform);
    free(catalog);
    free(before);
    free(content);
    free(old);
    umi_fs_free_text(log);
    return failed;
}
