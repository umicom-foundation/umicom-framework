/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/process_catalog.c
 * PURPOSE: Own copied process rows with explicit partial-scan reporting and creation-identity checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "process_scan_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiDesktopProcessCatalog
{
    UmiDesktopProcessReport report;
    size_t count;
    UmiDesktopSystemProcess items[UMI_DESKTOP_SYSTEM_SCAN_LIMIT];
};
static UmiStatus ProcessKeep(void *context, const UmiDesktopSystemProcess *process)
{
    UmiDesktopProcessCatalog *catalog = context;
    if (catalog->count >= UMI_DESKTOP_SYSTEM_SCAN_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalog->items[catalog->count++] = *process;
    return UMI_STATUS_OK;
}
static int ProcessCompare(const void *left, const void *right)
{
    const UmiDesktopSystemProcess *a = left, *b = right;
    return a->pid < b->pid ? -1 : a->pid > b->pid ? 1 : 0;
}
UmiStatus UmiDesktopProcessCatalogCreate(const UmiDesktopSystemProcess *rows, size_t count,
                                         const UmiDesktopProcessReport *report,
                                         UmiDesktopProcessCatalog **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (report == NULL || (count != 0U && rows == NULL) || report->seen < count)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DESKTOP_SYSTEM_SCAN_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDesktopProcessCatalog *catalog = calloc(1U, sizeof *catalog);
    if (catalog == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    catalog->report = *report;
    catalog->count = count;
    for (size_t i = 0U; i < count; ++i)
    {
        const char *end = memchr(rows[i].name, 0, sizeof rows[i].name);
        if (end == NULL)
        {
            free(catalog);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        catalog->items[i] = rows[i];
        UmiDesktopSystemDisplayName(catalog->items[i].name, sizeof catalog->items[i].name,
                                    rows[i].name, (size_t)(end - rows[i].name));
    }
    qsort(catalog->items, count, sizeof catalog->items[0], ProcessCompare);
    for (size_t i = 1U; i < count; ++i)
        if (catalog->items[i - 1U].pid == catalog->items[i].pid)
        {
            free(catalog);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    *out = catalog;
    return UMI_STATUS_OK;
}
UmiStatus UmiDesktopProcessCatalogCapture(const UmiDesktopProcessCaptureOptions *options,
                                          UmiDesktopProcessCatalog **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiDesktopProcessCaptureOptions defaults = {0};
    if (options == NULL)
        options = &defaults;
    if (options->linux_proc_root != NULL)
    {
        size_t length = strlen(options->linux_proc_root);
        if (length == 0U || length >= UMI_DESKTOP_SYSTEM_PATH_CAPACITY ||
            options->linux_proc_root[0] != '/')
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < length; ++i)
            if ((unsigned char)options->linux_proc_root[i] < 32U ||
                options->linux_proc_root[i] == 127)
                return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (options->cancelled != NULL && options->cancelled(options->context))
        return UMI_STATUS_CANCELLED;
    UmiDesktopProcessCatalog *catalog = calloc(1U, sizeof *catalog);
    if (catalog == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    catalog->report.captured_milliseconds = UmiDesktopSystemClock();
    UmiStatus status = UmiDesktopProcessScanNative(options, ProcessKeep, catalog, &catalog->report);
    uint64_t end = UmiDesktopSystemClock();
    if (end >= catalog->report.captured_milliseconds)
        catalog->report.elapsed_milliseconds = end - catalog->report.captured_milliseconds;
    /* A scan bound has a visible meaning: the rows collected so far are useful,
     * but the catalogue must never describe itself as the whole machine. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED)
    {
        catalog->report.limited = 1;
        status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK && options->cancelled != NULL &&
        options->cancelled(options->context))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        qsort(catalog->items, catalog->count, sizeof catalog->items[0], ProcessCompare);
        *out = catalog;
        return UMI_STATUS_OK;
    }
    free(catalog);
    return status;
}
void UmiDesktopProcessCatalogDestroy(UmiDesktopProcessCatalog *catalog) { free(catalog); }
UmiStatus UmiDesktopProcessCatalogReport(const UmiDesktopProcessCatalog *catalog,
                                         UmiDesktopProcessReport *out)
{
    if (catalog == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = catalog->report;
    return UMI_STATUS_OK;
}
size_t UmiDesktopProcessCatalogCount(const UmiDesktopProcessCatalog *catalog)
{
    return catalog != NULL ? catalog->count : 0U;
}
UmiStatus UmiDesktopProcessCatalogAt(const UmiDesktopProcessCatalog *catalog, size_t index,
                                     UmiDesktopSystemProcess *out)
{
    if (catalog == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalog->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalog->items[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiDesktopProcessCatalogFind(const UmiDesktopProcessCatalog *catalog, uint64_t pid,
                                       UmiDesktopSystemProcess *out)
{
    if (catalog == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t first = 0U, last = catalog->count;
    while (first < last)
    {
        size_t mid = first + (last - first) / 2U;
        if (catalog->items[mid].pid < pid)
            first = mid + 1U;
        else
            last = mid;
    }
    if (first == catalog->count || catalog->items[first].pid != pid)
        return UMI_STATUS_NOT_FOUND;
    *out = catalog->items[first];
    return UMI_STATUS_OK;
}
int UmiDesktopProcessIdentityEqual(const UmiDesktopSystemProcess *expected,
                                   const UmiDesktopSystemProcess *current)
{
    return expected != NULL && current != NULL && expected->pid != 0U && expected->startKnown &&
           current->startKnown && expected->pid == current->pid &&
           expected->startTicks == current->startTicks;
}
UmiStatus UmiDesktopProcessValidateCurrent(const UmiDesktopSystemProcess *expected)
{
    if (expected == NULL || expected->pid == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!expected->startKnown)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDesktopSystemProcess current;
    UmiStatus status = UmiDesktopProcessReadNative(expected->pid, &current);
    if (status != UMI_STATUS_OK)
        return status;
    return UmiDesktopProcessIdentityEqual(expected, &current) ? UMI_STATUS_OK
                                                              : UMI_STATUS_INVALID_STATE;
}
