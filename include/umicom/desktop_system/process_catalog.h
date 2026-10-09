/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/process_catalog.h
 * PURPOSE: Capture a bounded process catalogue and recheck process creation identity without reading command lines.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_PROCESS_CATALOG_H
#define UMICOM_DESKTOP_SYSTEM_PROCESS_CATALOG_H
#include "umicom/desktop_system/monitor.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef int (*UmiDesktopProcessCancelled)(void *context);
    typedef struct UmiDesktopProcessCaptureOptions
    {
        /* NULL uses the real local process provider. An absolute alternate /proc
     * root is Linux-only fixture input and is always labelled as such. */
        const char *linux_proc_root;
        UmiDesktopProcessCancelled cancelled;
        void *context;
    } UmiDesktopProcessCaptureOptions;
    typedef struct UmiDesktopProcessReport
    {
        size_t seen, unreadable;
        uint64_t captured_milliseconds, elapsed_milliseconds;
        int fixture, limited;
    } UmiDesktopProcessReport;
    typedef struct UmiDesktopProcessCatalog UmiDesktopProcessCatalog;
    /** Capture up to the shared 4096-record scan bound on a worker thread.
 * Names, PIDs, parent PIDs and available creation times are observations, not
 * authority to attach or terminate. No command lines, environments or credentials
 * are read, and no process is launched or signalled. Enumeration is not atomic.
 * Cancellation is checked between records; OS calls themselves are not preempted.
 * Success returns an owned catalogue, possibly marked limited. Failure clears out. */
    UmiStatus UmiDesktopProcessCatalogCapture(const UmiDesktopProcessCaptureOptions *options,
                                              UmiDesktopProcessCatalog **out);
    /** Copy observations from another process provider without querying the host.
 * Rows are sorted by PID and duplicates are rejected. Names must be terminated
 * within their fixed buffer; non-ASCII/control bytes become '?' for display,
 * matching native desktop monitoring. Mark fixture input in the supplied report.
 * The report's seen count must cover the supplied rows. These observations do
 * not establish local-host identity or grant debugger authority. Failure clears out. */
    UmiStatus UmiDesktopProcessCatalogCreate(const UmiDesktopSystemProcess *rows, size_t count,
                                             const UmiDesktopProcessReport *report,
                                             UmiDesktopProcessCatalog **out);
    /** Release an owned catalogue; accepts NULL. */
    void UmiDesktopProcessCatalogDestroy(UmiDesktopProcessCatalog *catalog);
    /** Read the copied capture report. Failure preserves out. */
    UmiStatus UmiDesktopProcessCatalogReport(const UmiDesktopProcessCatalog *catalog,
                                             UmiDesktopProcessReport *out);
    /** Return the number of copied rows, sorted by numeric PID. NULL has zero rows. */
    size_t UmiDesktopProcessCatalogCount(const UmiDesktopProcessCatalog *catalog);
    /** Copy one row without retaining borrowed pointers. Failure preserves out. */
    UmiStatus UmiDesktopProcessCatalogAt(const UmiDesktopProcessCatalog *catalog, size_t index,
                                         UmiDesktopSystemProcess *out);
    /** Find a captured PID without querying the host. Failure preserves out. */
    UmiStatus UmiDesktopProcessCatalogFind(const UmiDesktopProcessCatalog *catalog, uint64_t pid,
                                           UmiDesktopSystemProcess *out);
    /** Compare known creation identities. Unknown creation times are never considered equal.
 * Both rows must come from the same host/provider; this is not a cross-machine ID. */
    int UmiDesktopProcessIdentityEqual(const UmiDesktopSystemProcess *expected,
                                       const UmiDesktopSystemProcess *current);
    /** Read one current local PID and require its creation identity to match.
 * Unknown captured creation time returns NOT_IMPLEMENTED. A changed identity
 * returns INVALID_STATE; inaccessible/exited processes retain the provider error.
 * No process is attached or signalled. A PID can still be reused after this check
 * and before an external debugger acts, so this does not eliminate that race. */
    UmiStatus UmiDesktopProcessValidateCurrent(const UmiDesktopSystemProcess *expected);
#ifdef __cplusplus
}
#endif
#endif
