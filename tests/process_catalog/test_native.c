/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_catalog/test_native.c
 * PURPOSE: Inspect the test process creation identity without attaching, starting or signalling another process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/process_catalog.h"
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif
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
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (strcmp(argv[1], "current") && strcmp(argv[1], "reused") && strcmp(argv[1], "sorted"))
        return 2;
#if defined(_WIN32) || defined(__linux__)
#if defined(_WIN32)
    uint64_t pid = GetCurrentProcessId();
#else
    uint64_t pid = (uint64_t)getpid();
#endif
    UmiDesktopProcessCatalog *catalog = NULL;
    CHECK(UmiDesktopProcessCatalogCapture(NULL, &catalog) == UMI_STATUS_OK);
    UmiDesktopProcessReport report;
    CHECK(UmiDesktopProcessCatalogReport(catalog, &report) == UMI_STATUS_OK && !report.fixture);
    uint64_t last = 0U;
    for (size_t i = 0U; i < UmiDesktopProcessCatalogCount(catalog); ++i)
    {
        UmiDesktopSystemProcess row;
        CHECK(UmiDesktopProcessCatalogAt(catalog, i, &row) == UMI_STATUS_OK);
        CHECK(i == 0U || row.pid > last);
        last = row.pid;
    }
    UmiDesktopSystemProcess current;
    UmiStatus status = UmiDesktopProcessCatalogFind(catalog, pid, &current);
    if (status == UMI_STATUS_NOT_FOUND && report.limited)
    {
        puts("Process scan bound excluded this test process.");
        UmiDesktopProcessCatalogDestroy(catalog);
        return 77;
    }
    CHECK(status == UMI_STATUS_OK && current.startKnown);
    if (strcmp(argv[1], "reused") == 0)
    {
        current.startTicks ^= 1U;
        CHECK(UmiDesktopProcessValidateCurrent(&current) == UMI_STATUS_INVALID_STATE);
    }
    else
        CHECK(UmiDesktopProcessValidateCurrent(&current) == UMI_STATUS_OK);
    UmiDesktopProcessCatalogDestroy(catalog);
    return 0;
#else
    puts("Native process discovery is available on Windows and Linux.");
    return 77;
#endif
}
