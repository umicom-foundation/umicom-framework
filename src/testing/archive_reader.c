/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_reader.c
 * PURPOSE: Own asynchronous history observations independently of controls and database lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include "umicom/testing/archive_reader.h"
#include <stdlib.h>
struct UmiTestArchiveReader
{
    UmiTask *task;
    UmiMutex *mutex;
    UmiTestArchive *archive;
    uint64_t owner;
    UmiTestArchiveReaderSnapshot snapshot;
    UmiTestArchiveCatalog *catalog;
    UmiTestArchiveAttempt *attempt;
};
static bool reader_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}
static UmiStatus reader_run(UmiTaskContext *context, void *data)
{
    UmiTestArchiveReader *reader = data;
    const UmiCancellationToken *token = UmiTaskContextCancellation(context);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!umi_cancellation_token_is_requested(token))
    {
        if (reader->snapshot.kind == UMI_TEST_ARCHIVE_READ_CATALOG)
            status = UmiTestArchiveList(reader->archive, reader->catalog);
        else
            status = UmiTestArchiveReadAttempt(reader->archive, reader->snapshot.id,
                                               reader->snapshot.index, token, reader->attempt);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(token))
        status = UMI_STATUS_CANCELLED;
    /* Only the worker writes result storage. Readers can copy it after ready
     * is published under the mutex; an incomplete read is never exposed. */
    (void)umi_mutex_lock(reader->mutex);
    reader->snapshot.status = status;
    reader->snapshot.ready = status == UMI_STATUS_OK;
    (void)umi_mutex_unlock(reader->mutex);
    return status;
}
UmiStatus UmiTestArchiveReaderCreate(UmiTestArchive *archive, UmiTestArchiveReadKind kind,
                                     uint64_t id, size_t index, UmiTestArchiveReader **out_reader)
{
    if (out_reader == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_reader = NULL;
    if (archive == NULL ||
        (kind != UMI_TEST_ARCHIVE_READ_CATALOG && kind != UMI_TEST_ARCHIVE_READ_ATTEMPT) ||
        (kind == UMI_TEST_ARCHIVE_READ_CATALOG && (id != 0 || index != 0)) ||
        (kind == UMI_TEST_ARCHIVE_READ_ATTEMPT && (id == 0 || index >= UMI_CTEST_JOB_MAX_ATTEMPTS)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveReader *reader = calloc(1U, sizeof(*reader));
    if (reader == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    reader->archive = archive;
    reader->owner = umi_thread_current_id();
    reader->snapshot.kind = kind;
    reader->snapshot.id = id;
    reader->snapshot.index = index;
    reader->snapshot.status = UMI_STATUS_BUSY;
    if (kind == UMI_TEST_ARCHIVE_READ_CATALOG)
        reader->catalog = calloc(1U, sizeof(*reader->catalog));
    else
        reader->attempt = calloc(1U, sizeof(*reader->attempt));
    UmiStatus status = (reader->catalog == NULL && reader->attempt == NULL)
                           ? UMI_STATUS_OUT_OF_MEMORY
                           : umi_mutex_create(&reader->mutex);
    if (status == UMI_STATUS_OK)
    {
        UmiTaskConfig config = {0};
        config.label = "Read saved test evidence";
        config.function = reader_run;
        config.user_data = reader;
        status = umi_task_create(&config, &reader->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(reader->mutex);
        free(reader->catalog);
        free(reader->attempt);
        free(reader);
        return status;
    }
    *out_reader = reader;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveReaderSubmit(UmiTestArchiveReader *reader, UmiTaskQueue *queue)
{
    if (reader == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (reader->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, reader->task);
}
UmiStatus UmiTestArchiveReaderCancel(UmiTestArchiveReader *reader)
{
    return reader != NULL ? umi_task_cancel(reader->task) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiTestArchiveReaderRead(UmiTestArchiveReader *reader, UmiTestArchiveReaderSnapshot *out)
{
    if (reader == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTaskState state = umi_task_state(reader->task);
    (void)umi_mutex_lock(reader->mutex);
    UmiTestArchiveReaderSnapshot snapshot = reader->snapshot;
    (void)umi_mutex_unlock(reader->mutex);
    snapshot.state = state;
    if (reader_terminal(state) && snapshot.status == UMI_STATUS_BUSY)
        snapshot.status = umi_task_result(reader->task);
    *out = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveReaderCatalog(UmiTestArchiveReader *reader, UmiTestArchiveCatalog *out)
{
    if (reader == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(reader->mutex);
    UmiStatus status = reader->snapshot.kind != UMI_TEST_ARCHIVE_READ_CATALOG
                           ? UMI_STATUS_INVALID_STATE
                       : reader->snapshot.ready ? UMI_STATUS_OK
                                                : UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        *out = *reader->catalog;
    (void)umi_mutex_unlock(reader->mutex);
    return status;
}
UmiStatus UmiTestArchiveReaderAttempt(UmiTestArchiveReader *reader, UmiTestArchiveAttempt *out)
{
    if (reader == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(reader->mutex);
    UmiStatus status = reader->snapshot.kind != UMI_TEST_ARCHIVE_READ_ATTEMPT
                           ? UMI_STATUS_INVALID_STATE
                       : reader->snapshot.ready ? UMI_STATUS_OK
                                                : UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        *out = *reader->attempt;
    (void)umi_mutex_unlock(reader->mutex);
    return status;
}
UmiStatus UmiTestArchiveReaderDestroy(UmiTestArchiveReader *reader)
{
    if (reader == NULL)
        return UMI_STATUS_OK;
    if (reader->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(reader->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    umi_task_destroy(reader->task);
    umi_mutex_destroy(reader->mutex);
    free(reader->catalog);
    free(reader->attempt);
    free(reader);
    return UMI_STATUS_OK;
}
