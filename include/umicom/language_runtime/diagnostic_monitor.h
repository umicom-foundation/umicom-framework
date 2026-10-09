/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/diagnostic_monitor.h
 * PURPOSE: Coordinate live diagnostic snapshots between an editor thread and one persistent protocol worker.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_MONITOR_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_MONITOR_H
#include "umicom/language_runtime/diagnostic_session.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageDiagnosticMonitor UmiLanguageDiagnosticMonitor;
    typedef struct UmiLanguageDiagnosticMonitorConfig
    {
        UmiLanguageServerProfile profile;
        const char *working_directory, *tool_directory;
        UmiLanguageDiagnosticRequest document;
    } UmiLanguageDiagnosticMonitorConfig;
    typedef struct UmiLanguageDiagnosticMonitorSnapshot
    {
        uint64_t accepted_sequence, sent_sequence, publication_sequence, coalesced_updates;
        int started, ready, completed, stop_requested;
        UmiStatus status;
        UmiLanguageDiagnosticSessionSnapshot session;
    } UmiLanguageDiagnosticMonitorSnapshot;
    /** Copy the selected profile, paths and initial draft without starting a process.
 * The initial accepted sequence is 1. Creating a monitor does not grant trust.
 * Source/wire bounds are the session's limits. The caller owns the returned
 * monitor and must join its Run worker before Destroy. */
    UmiStatus UmiLanguageDiagnosticMonitorCreate(const UmiLanguageDiagnosticMonitorConfig *config,
                                                 UmiLanguageDiagnosticMonitor **out);
    /** Run once on a dedicated persistent worker until Stop or a protocol failure.
 * Submit, Read, Take and Stop may run concurrently. All native I/O and session
 * operations remain on this worker; no widget or editor pointer is retained.
 * A stopped monitor cannot restart. Create a new owner after joining the old one. */
    UmiStatus UmiLanguageDiagnosticMonitorRun(UmiLanguageDiagnosticMonitor *monitor);
    /** Same worker contract with an exclusive caller-owned STARTING test/host server.
 * Once initialization begins, the worker stops its transport. A preflight or
 * already-cancelled request leaves the borrowed server untouched. The caller
 * destroys the server object after Run returns. The monitor still uses its copied document/profile selection. */
    UmiStatus UmiLanguageDiagnosticMonitorRunOnServer(UmiLanguageDiagnosticMonitor *monitor,
                                                      UmiLanguageRuntimeServer *server);
    /** Queue a copied source snapshot; the newest pending source supersedes an
 * unsent older one. This intentionally coalesces typing, not editor history.
 * Identical text returns its existing sequence without invalidating diagnostics.
 * An accepted change immediately retires any unread publication. Output changes
 * only on success; invalid text or capacity failure keeps the preceding state.
 * After a failed update the host must stop displaying diagnostics for that draft. */
    UmiStatus UmiLanguageDiagnosticMonitorSubmit(UmiLanguageDiagnosticMonitor *monitor,
                                                 const char *source, size_t bytes,
                                                 uint64_t *out_sequence);
    /** Move an unread catalogue to the caller with the exact accepted source sequence.
 * NOT_FOUND means no current unread publication. Failure clears the catalogue and
 * leaves sequence unchanged. A host compares the sequence to its current captured
 * source before displaying it, and discards previously taken results on an edit.
 * Caller destroys the returned catalogue. */
    UmiStatus UmiLanguageDiagnosticMonitorTake(UmiLanguageDiagnosticMonitor *monitor,
                                               uint64_t *out_sequence,
                                               UmiLanguageDiagnosticCatalogue **out);
    /** Copy coherent state without waiting for native I/O. */
    UmiStatus UmiLanguageDiagnosticMonitorRead(UmiLanguageDiagnosticMonitor *monitor,
                                               UmiLanguageDiagnosticMonitorSnapshot *out);
    /** Request cooperative stop without blocking the editor. Initialization/poll
 * reads observe cancellation in short slices; native launches/writes and cleanup
 * still have the existing session's limits. */
    UmiStatus UmiLanguageDiagnosticMonitorStop(UmiLanguageDiagnosticMonitor *monitor);
    /** Release a never-started or joined monitor and clear its owning pointer.
 * BUSY preserves it while Run is active. completed is not a substitute for joining
 * the caller's thread/task before destroying data that worker might still use. */
    UmiStatus UmiLanguageDiagnosticMonitorDestroy(UmiLanguageDiagnosticMonitor **owner);
#ifdef __cplusplus
}
#endif
#endif
