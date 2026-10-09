/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_catalog/test_linux_fixture.c
 * PURPOSE: Check proc-directory observation, unreadable records, cancellation and scan bounds without touching real processes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/desktop_system/process_catalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            UmiDesktopProcessCatalogDestroy(catalog);                                              \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static int WriteProcess(const char *root, unsigned pid, unsigned actual)
{
    char path[4096];
    int n = snprintf(path, sizeof path, "%s/%u", root, pid);
    if (n < 0 || (size_t)n >= sizeof path || mkdir(path, 0700) != 0)
        return 0;
    n = snprintf(path, sizeof path, "%s/%u/stat", root, pid);
    if (n < 0 || (size_t)n >= sizeof path)
        return 0;
    FILE *file = fopen(path, "wb");
    if (file == NULL)
        return 0;
    int good =
        fprintf(file,
                "%u (fixture) S 1 2 3 4 5 6 7 8 9 10 11 12 13 -14 15 16 17 18 777 888 3 0 0\n",
                actual) > 0;
    if (fclose(file) != 0)
        good = 0;
    return good;
}
static int CancelAfter(void *context)
{
    unsigned *calls = context;
    return ++*calls >= 5U;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    unsigned count = 3U;
    if (strcmp(mode, "limit") == 0)
        count = UMI_DESKTOP_SYSTEM_SCAN_LIMIT + 1U;
    else if (strcmp(mode, "capture") && strcmp(mode, "mismatch") && strcmp(mode, "cancel"))
        return 2;
    char root[] = "/tmp/umicom-process-catalog-XXXXXX";
    UmiDesktopProcessCatalog *catalog = NULL;
    CHECK(mkdtemp(root) != NULL);
    for (unsigned i = 1U; i <= count; ++i)
        CHECK(WriteProcess(root, i, strcmp(mode, "mismatch") == 0 && i == 2U ? 99U : i));
    unsigned calls = 0;
    UmiDesktopProcessCaptureOptions options = {.linux_proc_root = root};
    if (strcmp(mode, "cancel") == 0)
    {
        options.cancelled = CancelAfter;
        options.context = &calls;
    }
    UmiStatus status = UmiDesktopProcessCatalogCapture(&options, &catalog);
    if (strcmp(mode, "cancel") == 0)
        CHECK(status == UMI_STATUS_CANCELLED && catalog == NULL);
    else
    {
        CHECK(status == UMI_STATUS_OK);
        UmiDesktopProcessReport report;
        CHECK(UmiDesktopProcessCatalogReport(catalog, &report) == UMI_STATUS_OK && report.fixture);
        CHECK(report.limited == (strcmp(mode, "limit") == 0));
        CHECK(report.unreadable == (strcmp(mode, "mismatch") == 0 ? 1U : 0U));
        CHECK(UmiDesktopProcessCatalogCount(catalog) == (strcmp(mode, "limit") == 0
                                                             ? UMI_DESKTOP_SYSTEM_SCAN_LIMIT
                                                         : strcmp(mode, "mismatch") == 0 ? 2U
                                                                                         : 3U));
    }
    printf("Isolated proc fixture retained: %s\n", root);
    UmiDesktopProcessCatalogDestroy(catalog);
    return 0;
}
