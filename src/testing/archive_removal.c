/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_removal.c
 * PURPOSE: Own queued test-history removal without borrowing frontend controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/archive_removal.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>

struct UmiTestArchiveRemoval
{
    UmiTask *task;
    UmiMutex *mutex;
    UmiTestArchive *archive;
    uint64_t owner;
    UmiTestArchiveRemovalSnapshot snapshot;
};
static bool removal_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}
static UmiStatus removal_run(UmiTaskContext *context, void *data)
{
    UmiTestArchiveRemoval *removal = data;
    UmiStatus status = UmiTestArchiveRemoveWithCancellation(removal->archive, removal->snapshot.id,
                                                            UmiTaskContextCancellation(context));
    /* Publish the transaction outcome before task completion. The queue may mark
     * a task cancelled after storage committed; that cannot rewrite history. */
    (void)umi_mutex_lock(removal->mutex);
    removal->snapshot.status = status;
    removal->snapshot.committed = status == UMI_STATUS_OK;
    (void)umi_mutex_unlock(removal->mutex);
    return status;
}
UmiStatus UmiTestArchiveRemovalCreate(UmiTestArchive *archive, uint64_t id,
                                      UmiTestArchiveRemoval **out_removal)
{
    if (out_removal == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_removal = NULL;
    if (archive == NULL || id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveRemoval *removal = calloc(1U, sizeof(*removal));
    if (removal == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    removal->archive = archive;
    removal->owner = umi_thread_current_id();
    removal->snapshot.id = id;
    removal->snapshot.status = UMI_STATUS_BUSY;
    UmiStatus status = umi_mutex_create(&removal->mutex);
    if (status == UMI_STATUS_OK)
    {
        UmiTaskConfig config = {0};
        config.label = "Remove saved test evidence";
        config.function = removal_run;
        config.user_data = removal;
        status = umi_task_create(&config, &removal->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(removal->mutex);
        free(removal);
        return status;
    }
    *out_removal = removal;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveRemovalSubmit(UmiTestArchiveRemoval *removal, UmiTaskQueue *queue)
{
    if (removal == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (removal->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, removal->task);
}
UmiStatus UmiTestArchiveRemovalCancel(UmiTestArchiveRemoval *removal)
{
    return removal != NULL ? umi_task_cancel(removal->task) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiTestArchiveRemovalRead(UmiTestArchiveRemoval *removal,
                                    UmiTestArchiveRemovalSnapshot *out)
{
    if (removal == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTaskState state = umi_task_state(removal->task);
    (void)umi_mutex_lock(removal->mutex);
    UmiTestArchiveRemovalSnapshot copy = removal->snapshot;
    (void)umi_mutex_unlock(removal->mutex);
    copy.state = state;
    if (removal_terminal(state) && copy.status == UMI_STATUS_BUSY)
        copy.status = umi_task_result(removal->task);
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveRemovalDestroy(UmiTestArchiveRemoval *removal)
{
    if (removal == NULL)
        return UMI_STATUS_OK;
    if (removal->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(removal->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    umi_task_destroy(removal->task);
    umi_mutex_destroy(removal->mutex);
    free(removal);
    return UMI_STATUS_OK;
}
