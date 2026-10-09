/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_catalog/test_catalog.c
 * PURPOSE: Check copied process observations, bounded storage, output preservation and creation-identity comparison.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/process_catalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int Cancel(void *context)
{
    ++*(unsigned *)context;
    return 1;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiDesktopSystemProcess rows[3] = {
        {.pid = 30, .parentPid = 2, .startTicks = 100, .startKnown = 1, .name = "worker"},
        {.pid = 10, .startTicks = 200, .startKnown = 1, .name = "editor"},
        {.pid = 20, .name = "unavailable"}};
    UmiDesktopProcessReport report = {
        .seen = 3, .unreadable = 1, .captured_milliseconds = 72, .limited = 1};
    UmiDesktopProcessCatalog *catalog = NULL;
    if (strncmp(mode, "identity-", 9) == 0)
    {
        UmiDesktopSystemProcess other = rows[0];
        int expected = 0;
        if (strcmp(mode, "identity-equal") == 0)
            expected = 1;
        else if (strcmp(mode, "identity-reused") == 0)
            ++other.startTicks;
        else if (strcmp(mode, "identity-pid") == 0)
            ++other.pid;
        else if (strcmp(mode, "identity-unknown") == 0)
            other.startKnown = 0;
        else if (strcmp(mode, "identity-zero") == 0)
        {
            other.pid = 0;
            rows[0].pid = 0;
        }
        else if (strcmp(mode, "identity-null") == 0)
        {
            CHECK(!UmiDesktopProcessIdentityEqual(NULL, &other));
            return 0;
        }
        else
            return 2;
        CHECK(UmiDesktopProcessIdentityEqual(&rows[0], &other) == expected);
        return 0;
    }
    if (strcmp(mode, "cancel") == 0)
    {
        unsigned calls = 0;
        UmiDesktopProcessCaptureOptions options = {.cancelled = Cancel, .context = &calls};
        CHECK(UmiDesktopProcessCatalogCapture(&options, &catalog) == UMI_STATUS_CANCELLED);
        CHECK(catalog == NULL && calls == 1U);
        return 0;
    }
    if (strcmp(mode, "relative-root") == 0 || strcmp(mode, "control-root") == 0)
    {
        UmiDesktopProcessCaptureOptions options = {
            .linux_proc_root = strcmp(mode, "relative-root") == 0 ? "proc" : "/proc\nother"};
        CHECK(UmiDesktopProcessCatalogCapture(&options, &catalog) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(catalog == NULL);
        return 0;
    }
    if (strcmp(mode, "unknown-current") == 0)
    {
        CHECK(UmiDesktopProcessValidateCurrent(&rows[2]) == UMI_STATUS_NOT_IMPLEMENTED);
        return 0;
    }
    if (strcmp(mode, "zero-current") == 0)
    {
        rows[0].pid = 0;
        CHECK(UmiDesktopProcessValidateCurrent(&rows[0]) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    size_t count = 3;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "empty") == 0)
    {
        count = 0;
        report.seen = 0;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        rows[2].pid = 10;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "unterminated") == 0)
    {
        memset(rows[0].name, 'x', sizeof rows[0].name);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "seen") == 0)
    {
        report.seen = 2;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        count = UMI_DESKTOP_SYSTEM_SCAN_LIMIT + 1U;
        report.seen = count;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "display") == 0)
        strcpy(rows[0].name, "bad\n\xc3\xa9");
    else if (strcmp(mode, "fixture") == 0)
        report.fixture = 1;
    else if (strcmp(mode, "copy") != 0 && strcmp(mode, "find") != 0 &&
             strcmp(mode, "bounds") != 0 && strcmp(mode, "report") != 0 &&
             strcmp(mode, "null") != 0)
        return 2;
    if (strcmp(mode, "null") == 0)
    {
        CHECK(UmiDesktopProcessCatalogCreate(NULL, 3, &report, &catalog) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDesktopProcessCatalogCount(NULL) == 0U);
        UmiDesktopProcessCatalogDestroy(NULL);
        return 0;
    }
    CHECK(UmiDesktopProcessCatalogCreate(count ? rows : NULL, count, &report, &catalog) ==
          expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(catalog == NULL);
        return 0;
    }
    CHECK(UmiDesktopProcessCatalogCount(catalog) == count);
    if (count != 0U)
    {
        UmiDesktopSystemProcess found = {.pid = 999}, original = found;
        CHECK(UmiDesktopProcessCatalogAt(catalog, 3, &found) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&found, &original, sizeof found) == 0);
        CHECK(UmiDesktopProcessCatalogFind(catalog, 99, &found) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&found, &original, sizeof found) == 0);
        CHECK(UmiDesktopProcessCatalogAt(catalog, 0, &found) == UMI_STATUS_OK && found.pid == 10);
        rows[0].pid = 999;
        strcpy(rows[0].name, "changed outside");
        CHECK(UmiDesktopProcessCatalogFind(catalog, 30, &found) == UMI_STATUS_OK &&
              found.pid == 30);
        CHECK(strcmp(found.name, strcmp(mode, "display") == 0 ? "bad???" : "worker") == 0);
    }
    UmiDesktopProcessReport copied = {0};
    CHECK(UmiDesktopProcessCatalogReport(catalog, &copied) == UMI_STATUS_OK);
    CHECK(copied.seen == report.seen && copied.unreadable == report.unreadable &&
          copied.limited == report.limited && copied.fixture == report.fixture);
    UmiDesktopProcessCatalogDestroy(catalog);
    return 0;
}
