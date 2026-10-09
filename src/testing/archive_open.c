/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_open.c
 * PURPOSE: Keep archive storage acquisition off event threads and transfer complete ownership once.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/output_file.h"
#include "umicom/platform/threading.h"
#include "umicom/testing/archive_open.h"
#include <stdlib.h>
#include <string.h>
struct UmiTestArchiveOpen
{
    UmiTask *task;
    UmiMutex *mutex;
    uint64_t owner;
    char scope[65];
    UmiTestArchiveOpenSnapshot snapshot;
    UmiDataServer *database;
    UmiTestArchive *archive;
};
static bool open_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}
static bool open_scope_valid(const char *scope)
{
    /* Check syntax before opening a file, using ArchiveCreate's key alphabet.
     * A caller cannot use a scope to select a filesystem location or a sibling
     * namespace. Keep this admission check aligned with the archive key rules. */
    if (scope == NULL || scope[0] == '\0')
        return false;
    for (size_t i = 0; i < 65U; ++i)
    {
        unsigned char c = (unsigned char)scope[i];
        if (c == '\0')
            return true;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
              c == '.' || c == '-' || c == '_'))
            return false;
    }
    return false;
}
static UmiStatus open_run(UmiTaskContext *context, void *data)
{
    UmiTestArchiveOpen *opening = data;
    const UmiCancellationToken *token = UmiTaskContextCancellation(context);
    UmiDataServer *database = NULL;
    UmiTestArchive *archive = NULL;
    UmiTestArchiveCatalog *catalog = NULL;
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!umi_cancellation_token_is_requested(token))
        status = umi_data_server_create_sqlite(opening->snapshot.path, &database);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(token))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = UmiTestArchiveCreate(database, opening->scope, &archive);
    if (status == UMI_STATUS_OK)
    {
        catalog = calloc(1U, sizeof(*catalog));
        status = catalog != NULL ? UmiTestArchiveList(archive, catalog) : UMI_STATUS_OUT_OF_MEMORY;
    }
    free(catalog);
    /* The stop flag and handle publication use the same lock. This closes the
     * interval between the final storage read and owner-thread attachment; a
     * late Stop must not silently replace the user's current archive. */
    (void)umi_mutex_lock(opening->mutex);
    if (opening->snapshot.stop_requested || umi_cancellation_token_is_requested(token))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        opening->database = database;
        database = NULL;
        opening->archive = archive;
        archive = NULL;
        opening->snapshot.ready = true;
    }
    opening->snapshot.status = status;
    (void)umi_mutex_unlock(opening->mutex);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(database);
    return status;
}
UmiStatus UmiTestArchiveOpenCreate(const char *path, const char *scope, UmiTestArchiveOpen **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!open_scope_valid(scope))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    if (strlen(path) >= UMI_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTestArchiveOpen *opening = calloc(1U, sizeof(*opening));
    if (opening == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    opening->owner = umi_thread_current_id();
    memcpy(opening->snapshot.path, path, strlen(path) + 1U);
    memcpy(opening->scope, scope, strlen(scope) + 1U);
    opening->snapshot.status = UMI_STATUS_BUSY;
    status = umi_mutex_create(&opening->mutex);
    if (status == UMI_STATUS_OK)
    {
        UmiTaskConfig config = {0};
        config.label = "Open private test archive";
        config.function = open_run;
        config.user_data = opening;
        status = umi_task_create(&config, &opening->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(opening->mutex);
        free(opening);
        return status;
    }
    *out = opening;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveOpenSubmit(UmiTestArchiveOpen *opening, UmiTaskQueue *queue)
{
    if (opening == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (opening->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, opening->task);
}
UmiStatus UmiTestArchiveOpenCancel(UmiTestArchiveOpen *opening)
{
    if (opening == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(opening->mutex);
    opening->snapshot.stop_requested = true;
    if (!opening->snapshot.taken)
    {
        opening->snapshot.ready = false;
        opening->snapshot.status = UMI_STATUS_CANCELLED;
    }
    (void)umi_mutex_unlock(opening->mutex);
    return umi_task_cancel(opening->task);
}
UmiStatus UmiTestArchiveOpenRead(UmiTestArchiveOpen *opening, UmiTestArchiveOpenSnapshot *out)
{
    if (opening == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTaskState state = umi_task_state(opening->task);
    (void)umi_mutex_lock(opening->mutex);
    UmiTestArchiveOpenSnapshot snapshot = opening->snapshot;
    (void)umi_mutex_unlock(opening->mutex);
    snapshot.state = state;
    if (open_terminal(state) && snapshot.status == UMI_STATUS_BUSY)
        snapshot.status = umi_task_result(opening->task);
    *out = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveOpenTake(UmiTestArchiveOpen *opening, UmiDataServer **database,
                                 UmiTestArchive **archive)
{
    if (opening == NULL || database == NULL || archive == NULL || *database != NULL ||
        *archive != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (opening->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(opening->task);
    if (!open_terminal(state))
        return state == UMI_TASK_CREATED ? UMI_STATUS_INVALID_STATE : UMI_STATUS_BUSY;
    (void)umi_mutex_lock(opening->mutex);
    UmiStatus status = opening->snapshot.taken            ? UMI_STATUS_NOT_FOUND
                       : opening->snapshot.stop_requested ? UMI_STATUS_CANCELLED
                       : opening->snapshot.ready          ? UMI_STATUS_OK
                                                          : opening->snapshot.status;
    if (status == UMI_STATUS_OK)
    {
        *database = opening->database;
        opening->database = NULL;
        *archive = opening->archive;
        opening->archive = NULL;
        opening->snapshot.taken = true;
        opening->snapshot.ready = false;
    }
    (void)umi_mutex_unlock(opening->mutex);
    if (status == UMI_STATUS_BUSY)
        status = umi_task_result(opening->task);
    return status;
}
UmiStatus UmiTestArchiveOpenDestroy(UmiTestArchiveOpen *opening)
{
    if (opening == NULL)
        return UMI_STATUS_OK;
    if (opening->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(opening->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    UmiTestArchiveDestroy(opening->archive);
    umi_data_server_destroy(opening->database);
    umi_task_destroy(opening->task);
    umi_mutex_destroy(opening->mutex);
    free(opening);
    return UMI_STATUS_OK;
}
