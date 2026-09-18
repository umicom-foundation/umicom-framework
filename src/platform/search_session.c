/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/search_session.c
 *
 * PURPOSE:
 *   Compose indexed text search with Framework tasks, cancellation and copied
 *   snapshots. Keep worker state separate from application presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/search_session.h"
#include "umicom/platform/task.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"

#include <stdlib.h>
#include <string.h>

struct UmiFileSearchSession {
    const UmiFileIndex *index;
    UmiTaskQueue *queue;
    UmiTask *task;
    UmiCancellationToken *cancellation;
    UmiMutex *mutex;
    UmiFileSearchSnapshot state;
    UmiSearchMatch *matches;
    size_t count;
};

/* The worker owns rows until completion. Readers never borrow their addresses. */
static UmiStatus CollectMatch(const UmiSearchMatch *match, void *userData)
{
    UmiFileSearchSession *session = userData;
    if (umi_cancellation_token_is_requested(session->cancellation))
        return UMI_STATUS_CANCELLED;
    if (session->count >= UMI_FILE_SEARCH_MAX_RESULTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    session->matches[session->count++] = *match;
    return UMI_STATUS_OK;
}

/* Only small statistics cross the worker/owner boundary during a search. */
static void PublishProgress(const UmiSearchStats *stats, void *userData)
{
    UmiFileSearchSession *session = userData;
    (void)umi_mutex_lock(session->mutex);
    session->state.stats = *stats;
    (void)umi_mutex_unlock(session->mutex);
}

static UmiStatus RunSearch(UmiTaskContext *context, void *userData)
{
    UmiFileSearchSession *session = userData;
    UmiSearchRequest request = umi_search_request_default(session->state.query);
    UmiSearchOptions options = {0};
    UmiSearchStats stats = {0};
    UmiStatus status;
    (void)context;
    request.case_sensitive = session->state.caseSensitive;
    request.cancellation = session->cancellation;
    request.maximum_results = UMI_FILE_SEARCH_MAX_RESULTS;
    request.maximum_file_size = UMI_FILE_SEARCH_MAX_FILE_BYTES;
    options.expectedRevision = session->state.indexRevision;
    options.progress = PublishProgress;
    options.progressUserData = session;
    status = UmiSearchFileIndexWithOptions(session->index, &request, &options,
        CollectMatch, session, &stats);
    (void)umi_mutex_lock(session->mutex);
    session->state.stats = stats;
    session->state.status = session->state.stale ? UMI_STATUS_BUSY : status;
    session->state.ready = !session->state.stale && status == UMI_STATUS_OK;
    (void)umi_mutex_unlock(session->mutex);
    return status;
}

UmiStatus UmiFileSearchCreate(const UmiFileIndex *index,
    UmiFileSearchSession **outSession)
{
    UmiFileSearchSession *session;
    UmiStatus status;
    if (outSession == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outSession = NULL;
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->index = index;
    status = umi_mutex_create(&session->mutex);
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&session->cancellation);
    if (status != UMI_STATUS_OK) { UmiFileSearchDestroy(session); return status; }
    *outSession = session;
    return UMI_STATUS_OK;
}

void UmiFileSearchDestroy(UmiFileSearchSession *session)
{
    if (session == NULL) return;
    umi_cancellation_token_request(session->cancellation);
    /* Drain rather than cancel the queued task: its function owns final state. */
    if (session->queue != NULL) (void)umi_task_queue_shutdown(session->queue, 0);
    umi_task_destroy(session->task);
    umi_task_queue_destroy(session->queue);
    umi_cancellation_token_destroy(session->cancellation);
    umi_mutex_destroy(session->mutex);
    free(session->matches);
    free(session);
}

UmiStatus UmiFileSearchStart(UmiFileSearchSession *session, const char *query,
    int caseSensitive, uint64_t expectedRevision)
{
    size_t length = 0U;
    char copiedQuery[UMI_FILE_SEARCH_QUERY_CAPACITY];
    UmiFileIndexPage page;
    UmiSearchMatch *rows;
    UmiTask *task = NULL;
    UmiTaskConfig taskConfig = {0};
    UmiStatus status;
    if (session == NULL || query == NULL || (caseSensitive != 0 && caseSensitive != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    while (length < sizeof copiedQuery && query[length] != '\0') ++length;
    if (length == 0U || length == sizeof copiedQuery ||
        memchr(query, '\n', length) != NULL || memchr(query, '\r', length) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(copiedQuery, query, length + 1U);
    if (session->queue != NULL) {
        UmiTaskQueueStats queued = umi_task_queue_stats(session->queue);
        if (queued.queued != 0U || queued.running != 0U) return UMI_STATUS_BUSY;
    }
    status = UmiFileIndexReadPage(session->index, "", 1, 0U,
        expectedRevision, NULL, 0U, &page);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_path_is_absolute(page.stats.root)) return UMI_STATUS_INVALID_STATE;
    if (session->state.requestId == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    rows = calloc(UMI_FILE_SEARCH_MAX_RESULTS, sizeof(*rows));
    if (rows == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (session->queue == NULL) {
        UmiTaskQueueConfig config = umi_task_queue_config_default();
        config.worker_count = 1U; config.capacity = 1U;
        status = umi_task_queue_create(&config, &session->queue);
    }
    taskConfig.label = "Find in Files";
    taskConfig.function = RunSearch; taskConfig.user_data = session;
    if (status == UMI_STATUS_OK) status = umi_task_create(&taskConfig, &task);
    if (status != UMI_STATUS_OK) { free(rows); return status; }
    umi_task_destroy(session->task);
    session->task = task;
    free(session->matches); session->matches = rows; session->count = 0U;
    umi_cancellation_token_reset(session->cancellation);
    (void)umi_mutex_lock(session->mutex);
    uint64_t requestId = session->state.requestId + 1U;
    memset(&session->state, 0, sizeof session->state);
    memcpy(session->state.query, copiedQuery, length + 1U);
    memcpy(session->state.root, page.stats.root, sizeof session->state.root);
    session->state.requestId = requestId;
    session->state.indexRevision = page.stats.revision;
    session->state.caseSensitive = caseSensitive;
    session->state.active = 1; session->state.status = UMI_STATUS_BUSY;
    (void)umi_mutex_unlock(session->mutex);
    status = umi_task_queue_submit(session->queue, task);
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_lock(session->mutex);
        session->state.active = 0; session->state.status = status;
        (void)umi_mutex_unlock(session->mutex);
    }
    return status;
}

UmiStatus UmiFileSearchCancel(UmiFileSearchSession *session)
{
    UmiFileSearchSnapshot state;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiFileSearchRead(session, &state);
    if (status != UMI_STATUS_OK) return status;
    if (state.active) umi_cancellation_token_request(session->cancellation);
    return UMI_STATUS_OK;
}

void UmiFileSearchInvalidate(UmiFileSearchSession *session)
{
    if (session == NULL) return;
    (void)umi_mutex_lock(session->mutex);
    if (session->state.requestId != 0U) {
        session->state.stale = 1;
        session->state.ready = 0;
        session->state.status = UMI_STATUS_BUSY;
        umi_cancellation_token_request(session->cancellation);
    }
    (void)umi_mutex_unlock(session->mutex);
}

UmiStatus UmiFileSearchRead(UmiFileSearchSession *session,
    UmiFileSearchSnapshot *outSnapshot)
{
    UmiFileIndexStats indexStats;
    UmiTaskQueueStats queued = {0};
    if (session == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->queue != NULL) queued = umi_task_queue_stats(session->queue);
    indexStats = umi_file_index_stats(session->index);
    (void)umi_mutex_lock(session->mutex);
    session->state.active = queued.queued != 0U || queued.running != 0U;
    if (session->state.requestId != 0U &&
        (indexStats.revision != session->state.indexRevision ||
         !umi_path_equal(indexStats.root, session->state.root))) {
        session->state.stale = 1; session->state.ready = 0;
        session->state.status = UMI_STATUS_BUSY;
        umi_cancellation_token_request(session->cancellation);
    }
    *outSnapshot = session->state;
    /* The queue still owns finalisation until idle, even when the task has
     * already published its own successful result. */
    if (outSnapshot->active) outSnapshot->ready = 0;
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

UmiStatus UmiFileSearchMatchAt(UmiFileSearchSession *session, uint64_t requestId,
    size_t position, UmiSearchMatch *outMatch)
{
    UmiFileSearchSnapshot state;
    if (session == NULL || outMatch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiFileSearchRead(session, &state);
    if (status != UMI_STATUS_OK) return status;
    if (requestId == 0U || requestId != state.requestId || state.stale)
        return UMI_STATUS_BUSY;
    if (!state.ready) return state.active ? UMI_STATUS_BUSY : UMI_STATUS_INVALID_STATE;
    if (position >= session->count) return UMI_STATUS_NOT_FOUND;
    *outMatch = session->matches[position];
    return UMI_STATUS_OK;
}

UmiStatus UmiFileSearchWait(UmiFileSearchSession *session,
    uint32_t timeoutMilliseconds)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return session->queue == NULL ? UMI_STATUS_OK :
        umi_task_queue_wait_idle(session->queue, timeoutMilliseconds);
}
