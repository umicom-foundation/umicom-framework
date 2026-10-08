/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_review.c
 * PURPOSE: Separate database comparison work from the lifetime of native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/archive_review.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>

struct UmiTestArchiveReview
{
    UmiTask *task;
    UmiMutex *mutex;
    uint64_t owner_thread, baseline_id, candidate_id;
    UmiTestArchive *archive;
    UmiTestArchiveComparison *comparison;
    UmiStatus status;
};
static bool review_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}
static UmiStatus review_run(UmiTaskContext *context, void *data)
{
    UmiTestArchiveReview *review = data;
    UmiTestArchiveComparison *comparison = NULL;
    UmiStatus status = UmiTestArchiveCompare(review->archive, review->baseline_id, review->candidate_id,
                                             UmiTaskContextCancellation(context), &comparison);
    /* Publish the immutable object before task completion. A late Stop must
     * not turn a successful database observation into a partial comparison. */
    (void)umi_mutex_lock(review->mutex);
    review->comparison = comparison;
    review->status = status;
    (void)umi_mutex_unlock(review->mutex);
    return status;
}
UmiStatus UmiTestArchiveReviewCreate(UmiTestArchive *archive, uint64_t baseline_id, uint64_t candidate_id,
                                     UmiTestArchiveReview **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (archive == NULL || baseline_id == 0 || candidate_id == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveReview *review = calloc(1U, sizeof(*review));
    if (review == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    review->archive = archive;
    review->baseline_id = baseline_id;
    review->candidate_id = candidate_id;
    review->owner_thread = umi_thread_current_id();
    review->status = UMI_STATUS_BUSY;
    UmiStatus status = umi_mutex_create(&review->mutex);
    if (status == UMI_STATUS_OK)
    {
        UmiTaskConfig config = {0};
        config.label = "Compare saved test outcomes";
        config.function = review_run;
        config.user_data = review;
        status = umi_task_create(&config, &review->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(review->mutex);
        free(review);
        return status;
    }
    *out = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveReviewSubmit(UmiTestArchiveReview *review, UmiTaskQueue *queue)
{
    if (review == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->owner_thread != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, review->task);
}
UmiStatus UmiTestArchiveReviewCancel(UmiTestArchiveReview *review)
{
    return review != NULL ? umi_task_cancel(review->task) : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiTestArchiveReviewRead(UmiTestArchiveReview *review, UmiTestArchiveReviewSnapshot *out)
{
    if (review == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveReviewSnapshot snapshot = {0};
    snapshot.state = umi_task_state(review->task);
    (void)umi_mutex_lock(review->mutex);
    snapshot.ready = review->comparison != NULL;
    snapshot.status = review->status;
    if (snapshot.ready)
        (void)UmiTestArchiveComparisonRead(review->comparison, &snapshot.summary);
    (void)umi_mutex_unlock(review->mutex);
    if (review_terminal(snapshot.state) && snapshot.status == UMI_STATUS_BUSY)
        snapshot.status = umi_task_result(review->task);
    *out = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveReviewRowAt(UmiTestArchiveReview *review, size_t index,
                                    UmiTestArchiveComparisonRow *out)
{
    if (review == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(review->mutex);
    UmiStatus status = review->comparison != NULL
                           ? UmiTestArchiveComparisonRowAt(review->comparison, index, out)
                           : UMI_STATUS_NOT_FOUND;
    (void)umi_mutex_unlock(review->mutex);
    return status;
}
UmiStatus UmiTestArchiveReviewDestroy(UmiTestArchiveReview *review)
{
    if (review == NULL)
        return UMI_STATUS_OK;
    if (review->owner_thread != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(review->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    umi_task_destroy(review->task);
    UmiTestArchiveComparisonDestroy(review->comparison);
    umi_mutex_destroy(review->mutex);
    free(review);
    return UMI_STATUS_OK;
}
