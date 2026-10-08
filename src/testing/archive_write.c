/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_write.c
 * PURPOSE: Own background archive work independently of frontend controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "archive_internal.h"
#include "umicom/testing/archive_write.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>

struct UmiTestArchiveWrite
{
    UmiTask *task;
    UmiMutex *mutex;
    uint64_t owner_thread;
    UmiTestArchive *archive;
    UmiCtestJob *source;
    UmiTestArchiveOrigin origin;
    UmiTestArchiveEntry saved;
    UmiStatus save_status;
};
static bool terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}
static UmiStatus write_run(UmiTaskContext *context, void *data)
{
    UmiTestArchiveWrite *writer = data;
    UmiTestArchiveEntry saved = {0};
    UmiStatus status = UmiTestArchiveSave(writer->archive, writer->source, &writer->origin,
                                          UmiTaskContextCancellation(context), &saved);
    /* Copy the committed outcome before task completion. A late cancellation
     * can affect task state but must not erase evidence that commit succeeded. */
    (void)umi_mutex_lock(writer->mutex);
    writer->saved = saved;
    writer->save_status = status;
    (void)umi_mutex_unlock(writer->mutex);
    return status;
}
UmiStatus UmiTestArchiveWriteCreate(UmiTestArchive *archive, UmiCtestJob *source,
                                    const UmiTestArchiveOrigin *origin, UmiTestArchiveWrite **out_writer)
{
    if (out_writer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_writer = NULL;
    if (archive == NULL || source == NULL || !UmiTestArchiveOriginValid(origin))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestJobSnapshot snapshot = {0};
    UmiStatus status = UmiCtestJobGetSnapshot(source, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (!terminal(snapshot.state))
        return UMI_STATUS_BUSY;
    UmiTestArchiveWrite *writer = calloc(1U, sizeof(*writer));
    if (writer == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    writer->archive = archive;
    writer->source = source;
    writer->origin = *origin;
    writer->owner_thread = umi_thread_current_id();
    writer->save_status = UMI_STATUS_BUSY;
    status = umi_mutex_create(&writer->mutex);
    if (status == UMI_STATUS_OK)
    {
        UmiTaskConfig config = {0};
        config.label = "Save completed test evidence";
        config.function = write_run;
        config.user_data = writer;
        status = umi_task_create(&config, &writer->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(writer->mutex);
        free(writer);
        return status;
    }
    *out_writer = writer;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveWriteSubmit(UmiTestArchiveWrite *writer, UmiTaskQueue *queue)
{
    if (writer == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (writer->owner_thread != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, writer->task);
}
UmiStatus UmiTestArchiveWriteCancel(UmiTestArchiveWrite *writer)
{
    return writer != NULL ? umi_task_cancel(writer->task) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiTestArchiveWriteRead(UmiTestArchiveWrite *writer, UmiTestArchiveWriteSnapshot *out)
{
    if (writer == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveWriteSnapshot snapshot = {0};
    snapshot.state = umi_task_state(writer->task);
    (void)umi_mutex_lock(writer->mutex);
    snapshot.saved = writer->saved;
    snapshot.status = writer->save_status;
    (void)umi_mutex_unlock(writer->mutex);
    if (terminal(snapshot.state) && snapshot.status == UMI_STATUS_BUSY)
        snapshot.status = umi_task_result(writer->task);
    *out = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveWriteDestroy(UmiTestArchiveWrite *writer)
{
    if (writer == NULL)
        return UMI_STATUS_OK;
    if (writer->owner_thread != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(writer->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    umi_task_destroy(writer->task);
    umi_mutex_destroy(writer->mutex);
    free(writer);
    return UMI_STATUS_OK;
}
