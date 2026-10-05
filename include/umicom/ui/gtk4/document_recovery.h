/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_recovery.h
 * PURPOSE: Expose local draft capture, discovery, preview and restore through one shared native form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DOCUMENT_RECOVERY_H
#define UMICOM_UI_GTK4_DOCUMENT_RECOVERY_H
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/document/recovery_schedule.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Open a local recovery form without reading/writing storage. Save snapshot
 * captures the active visible draft; Refresh lists stored records; Preview
 * reads one complete record; Restore opens it as a new unsaved tab. Storage
 * work runs off the GUI thread with owned inputs and an invalidatable owner.
 * The settings base is absolute or NULL for native per-user storage. No source
 * file is saved, no network is used, and no automatic periodic capture is
 * enabled by opening the form. Its explicit opt-in control enables periodic
 * capture for this binding. The shared completion Busy/Cancel functions include
 * this form; closing it leaves an opted-in timer running. */
    UmiStatus UmiGtk4AdapterReviewRecovery(UmiGtk4Adapter *adapter, const char *application_directory,
                                           const char *base_override);
    /* Periodic snapshots are opt-in for this editing binding only. No preference
 * is persisted. The timer observes all managed dirty drafts and captures one
 * due document at a time, in fair rotation. It skips unchanged revisions and
 * pauses on an error or a 256-record / 128 MiB admission limit. Nothing is
 * pruned. Re-enable explicitly to resume after resolving a failure. Directory
 * changes are refused while a worker is pending; unbinding retires the timer.
 * interval_ms is between one second and one hour. Default UI policy is a minute. */
    UmiStatus UmiGtk4AdapterConfigureRecovery(UmiGtk4Adapter *adapter, const char *application_directory,
                                              const char *base_override, int enabled, uint64_t interval_ms);
    UmiStatus UmiGtk4AdapterRecoveryState(UmiGtk4Adapter *adapter, UmiDocumentRecoveryScheduleInfo *out_info);
    /* Notify the host after configuration or a completed/failed capture, on the
 * owner thread. The copied status is borrowed only during the callback. The
 * callback may detach the binding; it must never be called afterwards. Bind
 * after document editing. No event reads or transmits source text. */
    typedef void (*UmiGtk4RecoveryStatusFn)(void *context, const UmiDocumentRecoveryScheduleInfo *info);
    UmiStatus UmiGtk4AdapterObserveRecovery(UmiGtk4Adapter *adapter, UmiGtk4RecoveryStatusFn observer,
                                            void *context);
#ifdef __cplusplus
}
#endif
#endif
