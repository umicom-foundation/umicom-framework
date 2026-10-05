/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/application_processes.h
 * PURPOSE: Observe independently launched native applications without retaining windows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_APPLICATION_PROCESSES_H
#define UMICOM_UI_GTK4_APPLICATION_PROCESSES_H
#include "umicom/application/launch_receipts.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiGtk4ApplicationProcesses UmiGtk4ApplicationProcesses;
    /* receipt is borrowed for this notification only. The observer may read copied
 * receipts or destroy the service; it must not recursively start applications. */
    typedef void (*UmiGtk4ApplicationProcessObserver)(const UmiApplicationLaunchReceipt *receipt,
                                                      void *context);

    /* Main-context owner only. Construction creates no process and touches no files.
 * Destruction disconnects the observer immediately, but never terminates an
 * independently opened application. Outstanding waits keep only internal state
 * alive until completion, not the caller's window or controller. */
    UmiStatus UmiGtk4ApplicationProcessesCreate(UmiGtk4ApplicationProcesses **out);
    void UmiGtk4ApplicationProcessesDestroy(UmiGtk4ApplicationProcesses *processes);
    UmiStatus UmiGtk4ApplicationProcessesObserve(UmiGtk4ApplicationProcesses *processes,
                                                 UmiGtk4ApplicationProcessObserver observer, void *context);
    /* Only a canonical Framework GUI executable at an absolute local path is
 * accepted. There are no shell commands, arbitrary arguments, PATH search or
 * automatic retries here. The existing caller resolves the trusted location.
 * The child works in its executable directory, independently of the caller's
 * current directory. A start is not a readiness handshake. Standard opens
 * refuse an already tracked active instance; New window must be explicit.
 * out_id remains unchanged before reservation; a spawn failure returns its
 * rejected receipt ID so the caller can inspect the result. */
    UmiStatus UmiGtk4ApplicationProcessesStart(UmiGtk4ApplicationProcesses *processes,
                                               const char *application_id, const char *absolute_executable,
                                               bool explicit_new_instance, uint64_t *out_id);
    UmiStatus UmiGtk4ApplicationProcessesLatest(const UmiGtk4ApplicationProcesses *processes,
                                                const char *application_id, UmiApplicationLaunchReceipt *out);
    UmiStatus UmiGtk4ApplicationProcessesAt(const UmiGtk4ApplicationProcesses *processes, size_t index,
                                            UmiApplicationLaunchReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
