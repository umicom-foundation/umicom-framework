/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/review.c
 *
 * PURPOSE:
 *   Coordinate copied scan jobs, context invalidation and retained report publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/review.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>

struct UmiCodeGuardReview {
    UmiCodeGuardService *service;
    UmiCodeGuardJob *job;
    uint64_t owner_thread;
    uint64_t sequence;
    uint64_t context_id;
    uint64_t report_context_id;
    UmiTaskState state;
    UmiStatus status;
    UmiStatus operation_status;
    int cancel_requested;
    int stale;
    int finalized;
    int has_report;
};

static UmiStatus review_owner(const UmiCodeGuardReview *review)
{
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return review->owner_thread == umi_thread_current_id()
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus UmiCodeGuardReviewCreate(const char *root, UmiCodeGuardReview **out_review)
{
    if (out_review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_review = NULL;
    if (root == NULL || !umi_path_is_absolute(root)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardReview *review = calloc(1U, sizeof *review);
    if (review == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_codeguard_service_create(root, &review->service);
    if (status != UMI_STATUS_OK) { free(review); return status; }
    review->owner_thread = umi_thread_current_id();
    review->state = UMI_TASK_CREATED;
    review->status = UMI_STATUS_NOT_FOUND;
    review->operation_status = UMI_STATUS_NOT_FOUND;
    *out_review = review;
    return UMI_STATUS_OK;
}

UmiStatus UmiCodeGuardReviewBegin(UmiCodeGuardReview *review, UmiTaskQueue *queue,
    const UmiCodeGuardProfile *profile, uint64_t context_id, uint64_t *out_request_id)
{
    if (queue == NULL || out_request_id == NULL || context_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = review_owner(review);
    if (status != UMI_STATUS_OK) return status;
    if (review->job != NULL) return UMI_STATUS_BUSY;
    if (review->sequence == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCodeGuardJob *candidate = NULL;
    status = umi_codeguard_service_create_job(review->service, profile, &candidate);
    if (status == UMI_STATUS_OK) status = UmiCodeGuardJobSubmit(candidate, queue);
    if (status != UMI_STATUS_OK) {
        /* Rejected jobs are still created, owned here and safe to release.
         * Do not change a previous report or consume a request number. */
        (void)UmiCodeGuardJobDestroy(&candidate);
        return status;
    }
    review->job = candidate;
    ++review->sequence;
    review->context_id = context_id;
    review->state = UMI_TASK_QUEUED;
    review->status = UMI_STATUS_BUSY;
    review->operation_status = UMI_STATUS_BUSY;
    review->cancel_requested = 0;
    review->stale = 0;
    review->finalized = 0;
    *out_request_id = review->sequence;
    return UMI_STATUS_OK;
}

UmiStatus UmiCodeGuardReviewCancel(UmiCodeGuardReview *review)
{
    UmiStatus status = review_owner(review);
    if (status != UMI_STATUS_OK || review->job == NULL) return status;
    status = UmiCodeGuardJobCancel(review->job);
    if (status == UMI_STATUS_OK && !review->finalized) review->cancel_requested = 1;
    return status;
}

/* A task's terminal state and its scan outcome are separate. In particular, a
 * cancelled attempt can retain a specific earlier file/allocation failure. */
static int review_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}

UmiStatus UmiCodeGuardReviewPoll(UmiCodeGuardReview *review, uint64_t context_id,
    UmiCodeGuardReviewSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = review_owner(review);
    if (status != UMI_STATUS_OK) return status;
    if (review->job != NULL) {
        /* Latch loss of authority. Returning to the old token cannot revive an
         * attempt already observed in another context. The host owns token
         * generation; pointer reuse or path spelling is not a session token. */
        if (context_id != review->context_id && !review->finalized) review->stale = 1;
        if (review->stale && !review->finalized) {
            status = UmiCodeGuardJobCancel(review->job);
            if (status != UMI_STATUS_OK) return status;
        }
        UmiCodeGuardJobSnapshot job;
        status = UmiCodeGuardJobRead(review->job, &job);
        if (status != UMI_STATUS_OK) return status;
        review->state = job.state;
        review->operation_status = job.operation_status;
        if (review_terminal(job.state)) {
            if (!review->finalized) {
                review->status = review->stale ? UMI_STATUS_INVALID_STATE : job.status;
                if (review->status == UMI_STATUS_OK && review->cancel_requested)
                    review->status = UMI_STATUS_CANCELLED;
                if (review->status == UMI_STATUS_OK) {
                    review->status = umi_codeguard_service_adopt_job(review->service, review->job);
                    if (review->status == UMI_STATUS_OK) {
                        review->has_report = 1;
                        review->report_context_id = review->context_id;
                    }
                }
                /* A retirement error must not cause a second take/publication
                 * on retry. Keep the completed publication outcome separately. */
                review->finalized = 1;
            }
            status = UmiCodeGuardJobDestroy(&review->job);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    UmiCodeGuardReviewSnapshot snapshot = {
        .request_id = review->sequence, .context_id = review->context_id,
        .report_context_id = review->report_context_id, .state = review->state,
        .status = review->status, .operation_status = review->operation_status,
        .pending = review->job != NULL, .cancel_requested = review->cancel_requested,
        .stale = review->stale,
        .has_report = context_id != 0U && review->has_report &&
            context_id == review->report_context_id
    };
    if (snapshot.has_report) snapshot.summary = umi_codeguard_service_summary(review->service);
    *out = snapshot;
    return UMI_STATUS_OK;
}

const UmiCodeGuardResult *UmiCodeGuardReviewResult(const UmiCodeGuardReview *review,
    uint64_t context_id)
{
    if (review_owner(review) != UMI_STATUS_OK || context_id == 0U ||
        !review->has_report || context_id != review->report_context_id) return NULL;
    return umi_codeguard_service_result(review->service);
}

UmiStatus UmiCodeGuardReviewDestroy(UmiCodeGuardReview **in_out_review)
{
    if (in_out_review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardReview *review = *in_out_review;
    if (review == NULL) return UMI_STATUS_OK;
    UmiStatus status = review_owner(review);
    if (status != UMI_STATUS_OK) return status;
    status = UmiCodeGuardJobDestroy(&review->job);
    if (status != UMI_STATUS_OK) return status;
    umi_codeguard_service_destroy(review->service);
    free(review);
    *in_out_review = NULL;
    return UMI_STATUS_OK;
}
