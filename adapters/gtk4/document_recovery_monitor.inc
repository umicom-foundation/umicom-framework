/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_recovery_monitor.inc
 * PURPOSE: Run opt-in recovery workers independently of forms while retiring authority with the editor binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_recovery.h"
#include "umicom/document/recovery_coordinator.h"
#include "umicom/document/recovery_storage.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
typedef struct RecoveryMonitor RecoveryMonitor;
typedef struct RecoveryAutomaticWork
{
    RecoveryMonitor *monitor;
    UmiCancellationToken *cancel;
    UmiDocumentRecoveryDraft *draft;
    UmiDocumentRecoveryTicket ticket;
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status;
} RecoveryAutomaticWork;
struct RecoveryMonitor
{
    unsigned references;
    GObject *lifetime;
    UmiDocumentRecoverySchedule *schedule;
    RecoveryAutomaticWork *pending;
    char directory[UMI_PATH_CAPACITY];
    guint timer;
    int retired;
};
typedef struct RecoveryObserver
{
    UmiGtk4RecoveryStatusFn callback;
    void *context;
} RecoveryObserver;
static void RecoveryPanelMonitorChanged(GObject *lifetime);
static uint64_t RecoveryNow(void)
{
    gint64 now = g_get_monotonic_time();
    return now < 0 ? 0U : (uint64_t)(now / 1000);
}
/* References are changed only on the GTK owner thread. Worker input carries
 * a retained monitor but never reads its live state; completion releases it. */
static RecoveryMonitor *RecoveryMonitorRef(RecoveryMonitor *monitor)
{
    ++monitor->references;
    return monitor;
}
static void RecoveryMonitorRelease(gpointer data)
{
    RecoveryMonitor *monitor = data;
    if (--monitor->references == 0U)
    {
        UmiDocumentRecoveryScheduleDestroy(monitor->schedule);
        g_object_unref(monitor->lifetime);
        g_free(monitor);
    }
}
static UmiGtk4Adapter *RecoveryMonitorOwner(RecoveryMonitor *monitor)
{
    if (monitor->retired || g_object_get_data(monitor->lifetime, "recovery-monitor") != monitor)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(monitor->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void RecoveryMonitorStopTimer(RecoveryMonitor *monitor)
{
    guint timer = monitor->timer;
    monitor->timer = 0U;
    if (timer != 0U)
        g_source_remove(timer);
}
static void RecoveryMonitorDetach(GObject *lifetime)
{
    RecoveryMonitor *monitor = g_object_get_data(lifetime, "recovery-monitor");
    if (monitor == NULL)
        return;
    RecoveryMonitorRef(monitor);
    monitor->retired = 1;
    if (monitor->pending != NULL)
        umi_cancellation_token_request(monitor->pending->cancel);
    RecoveryMonitorStopTimer(monitor);
    g_object_set_data(lifetime, "recovery-monitor", NULL);
    RecoveryMonitorRelease(monitor);
}
static void RecoveryMonitorNotify(RecoveryMonitor *monitor)
{
    if (RecoveryMonitorOwner(monitor) == NULL)
        return;
    RecoveryPanelMonitorChanged(monitor->lifetime);
    if (RecoveryMonitorOwner(monitor) == NULL)
        return;
    RecoveryObserver *observer = g_object_get_data(monitor->lifetime, "recovery-observer");
    UmiDocumentRecoveryScheduleInfo info;
    if (observer != NULL && observer->callback != NULL &&
        UmiDocumentRecoveryScheduleInspect(monitor->schedule, &info) == UMI_STATUS_OK)
    {
        UmiGtk4RecoveryStatusFn callback = observer->callback;
        void *context = observer->context;
        /* Host notification is last: it may destroy the adapter or runtime. */
        callback(context, &info);
    }
}
static void RecoveryAutomaticFree(RecoveryAutomaticWork *work)
{
    UmiDocumentRecoveryDraftDestroy(work->draft);
    umi_cancellation_token_destroy(work->cancel);
    RecoveryMonitorRelease(work->monitor);
    g_free(work);
}
static void RecoveryAutomaticWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    RecoveryAutomaticWork *work = data;
    work->status = UMI_STATUS_OK;
    if (umi_cancellation_token_is_requested(work->cancel))
        work->status = UMI_STATUS_CANCELLED;
    if (work->status == UMI_STATUS_OK)
    {
        GFile *directory = g_file_new_for_path(work->directory);
        GError *error = NULL;
        if (!g_file_make_directory_with_parents(directory, NULL, &error) &&
            !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS))
            work->status = UMI_STATUS_IO_ERROR;
        g_clear_error(&error);
        g_object_unref(directory);
    }
    if (work->status == UMI_STATUS_OK)
        work->status = UmiDocumentRecoverySaveWithinBudget(work->directory, work->draft,
                                                           UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT,
                                                           UINT64_C(128) * 1024U * 1024U, work->cancel);
    g_task_return_boolean(task, TRUE);
}
static void RecoveryAutomaticDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    RecoveryAutomaticWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    RecoveryMonitor *monitor = work->monitor;
    if (monitor->pending == work)
    {
        monitor->pending = NULL;
        if (RecoveryMonitorOwner(monitor) != NULL)
        {
            (void)UmiDocumentRecoveryScheduleFinish(monitor->schedule, &work->ticket, work->status,
                                                    RecoveryNow());
            if (work->status != UMI_STATUS_OK)
                RecoveryMonitorStopTimer(monitor);
            RecoveryMonitorNotify(monitor);
        }
    }
    RecoveryAutomaticFree(work);
}
static gboolean RecoveryMonitorTick(gpointer data)
{
    RecoveryMonitor *monitor = RecoveryMonitorRef(data);
    UmiGtk4Adapter *owner = RecoveryMonitorOwner(monitor);
    if (owner == NULL)
    {
        RecoveryMonitorDetach(monitor->lifetime);
        RecoveryMonitorRelease(monitor);
        return G_SOURCE_REMOVE;
    }
    UmiDocumentRecoveryScheduleInfo info;
    (void)UmiDocumentRecoveryScheduleInspect(monitor->schedule, &info);
    if (!info.enabled || info.paused_after_failure)
    {
        monitor->timer = 0U;
        RecoveryMonitorRelease(monitor);
        return G_SOURCE_REMOVE;
    }
    if (monitor->pending != NULL || owner->edit_cancel != NULL || UmiGtk4AdapterCompletionReviewBusy(owner) ||
        UmiGtk4AdapterReplacementReviewBusy(owner) || UmiGtk4AdapterDocumentCloseBusy(owner) ||
        UmiGtk4AdapterDocumentSaveAllBusy(owner))
    {
        RecoveryMonitorRelease(monitor);
        return G_SOURCE_CONTINUE;
    }
    uint64_t now = RecoveryNow();
    UmiDocumentRecoveryObservation observations[UMI_DOCUMENT_MAX_WORKING_COPIES];
    size_t count = 0U;
    UmiStatus status = UmiDocumentCoordinatorObserveRecovery(owner->edit_coordinator, observations,
                                                             UMI_DOCUMENT_MAX_WORKING_COPIES, &count);
    if (status == UMI_STATUS_BUSY)
    {
        RecoveryMonitorRelease(monitor);
        return G_SOURCE_CONTINUE;
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryScheduleObserve(monitor->schedule, observations, count, now);
    UmiDocumentRecoveryTicket ticket = {0};
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryScheduleBegin(monitor->schedule, now, &ticket);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        RecoveryMonitorRelease(monitor);
        return G_SOURCE_CONTINUE;
    }
    RecoveryAutomaticWork *work = NULL;
    if (status == UMI_STATUS_OK)
    {
        work = g_try_new0(RecoveryAutomaticWork, 1);
        if (work == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (work != NULL)
    {
        work->monitor = RecoveryMonitorRef(monitor);
        work->ticket = ticket;
        memcpy(work->directory, monitor->directory, sizeof(work->directory));
        status = umi_cancellation_token_create(&work->cancel);
        char key[UMI_DOCUMENT_RECOVERY_KEY_CAPACITY];
        if (status == UMI_STATUS_OK)
            status = UmiDocumentRecoveryKeyCreate(key, sizeof(key));
        if (status == UMI_STATUS_OK)
            status = UmiDocumentCoordinatorCaptureRecovery(
                owner->edit_coordinator, ticket.captured.document_id, key, work->cancel, &work->draft);
    }
    if (status == UMI_STATUS_OK)
    {
        /* Capture and observation run together on the owner thread. Only the
         * independent draft crosses to the worker; no source file is saved. */
        monitor->pending = work;
        GTask *task = g_task_new(NULL, NULL, RecoveryAutomaticDone, NULL);
        g_task_set_task_data(task, work, NULL);
        g_task_run_in_thread(task, RecoveryAutomaticWorker);
        g_object_unref(task);
    }
    else
    {
        if (ticket.sequence != 0U)
            (void)UmiDocumentRecoveryScheduleFinish(monitor->schedule, &ticket, status, now);
        else
            (void)UmiDocumentRecoveryScheduleSuspend(monitor->schedule, status, now);
        if (work != NULL)
            RecoveryAutomaticFree(work);
        RecoveryMonitorStopTimer(monitor);
        RecoveryMonitorNotify(monitor);
    }
    int active = !monitor->retired && monitor->timer != 0U;
    RecoveryMonitorRelease(monitor);
    return active ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}
UmiStatus UmiGtk4AdapterObserveRecovery(UmiGtk4Adapter *adapter, UmiGtk4RecoveryStatusFn callback,
                                        void *context)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL)
        return UMI_STATUS_UNAVAILABLE;
    RecoveryObserver *observer = NULL;
    if (callback != NULL)
    {
        observer = g_try_new0(RecoveryObserver, 1);
        if (observer == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        observer->callback = callback;
        observer->context = context;
    }
    g_object_set_data_full(adapter->edit_lifetime, "recovery-observer", observer, g_free);
    return UMI_STATUS_OK;
}
UmiStatus UmiGtk4AdapterRecoveryState(UmiGtk4Adapter *adapter, UmiDocumentRecoveryScheduleInfo *out)
{
    if (adapter == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL)
        return UMI_STATUS_UNAVAILABLE;
    RecoveryMonitor *monitor = g_object_get_data(adapter->edit_lifetime, "recovery-monitor");
    if (monitor == NULL)
    {
        memset(out, 0, sizeof(*out));
        out->interval_ms = 60000U;
        out->last_status = UMI_STATUS_OK;
        return UMI_STATUS_OK;
    }
    return UmiDocumentRecoveryScheduleInspect(monitor->schedule, out);
}
UmiStatus UmiGtk4AdapterConfigureRecovery(UmiGtk4Adapter *adapter, const char *application, const char *base,
                                          int enabled, uint64_t interval)
{
    if (adapter == NULL || (enabled != 0 && enabled != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL || adapter->window == NULL)
        return UMI_STATUS_UNAVAILABLE;
    if (!UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_CANCELLED;
    if (interval < UMI_DOCUMENT_RECOVERY_INTERVAL_MINIMUM_MS ||
        interval > UMI_DOCUMENT_RECOVERY_INTERVAL_MAXIMUM_MS)
        return UMI_STATUS_INVALID_ARGUMENT;
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status = UmiDocumentRecoveryDirectory(application, base, directory, sizeof(directory));
    if (status != UMI_STATUS_OK)
        return status;
    RecoveryMonitor *monitor = g_object_get_data(adapter->edit_lifetime, "recovery-monitor");
    if (monitor != NULL && strcmp(monitor->directory, directory) != 0)
    {
        if (monitor->pending != NULL)
            return UMI_STATUS_BUSY;
        RecoveryMonitorDetach(adapter->edit_lifetime);
        monitor = NULL;
    }
    if (monitor == NULL)
    {
        monitor = g_try_new0(RecoveryMonitor, 1);
        if (monitor == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        monitor->references = 1U;
        monitor->lifetime = g_object_ref(adapter->edit_lifetime);
        memcpy(monitor->directory, directory, sizeof(directory));
        status = UmiDocumentRecoveryScheduleCreate(&monitor->schedule);
        if (status != UMI_STATUS_OK)
        {
            RecoveryMonitorRelease(monitor);
            return status;
        }
        g_object_set_data_full(adapter->edit_lifetime, "recovery-monitor", monitor, RecoveryMonitorRelease);
    }
    RecoveryMonitorRef(monitor);
    status = UmiDocumentRecoveryScheduleConfigure(monitor->schedule, enabled, interval, RecoveryNow());
    if (status == UMI_STATUS_OK)
    {
        if (!enabled)
            RecoveryMonitorStopTimer(monitor);
        else if (monitor->timer == 0U)
            monitor->timer = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000U, RecoveryMonitorTick,
                                                RecoveryMonitorRef(monitor), RecoveryMonitorRelease);
        RecoveryMonitorNotify(monitor);
    }
    RecoveryMonitorRelease(monitor);
    return status;
}
