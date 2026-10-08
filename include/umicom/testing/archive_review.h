/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/archive_review.h
 * PURPOSE: Run a cancellable comparison while frontends read only copied memory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_ARCHIVE_REVIEW_H
#define UMICOM_TESTING_ARCHIVE_REVIEW_H
#include "umicom/testing/archive_compare.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiTestArchiveReview UmiTestArchiveReview;
    typedef struct UmiTestArchiveReviewSnapshot
    {
        UmiTaskState state;
        UmiStatus status;
        bool ready;
        UmiTestArchiveComparisonSummary summary;
    } UmiTestArchiveReviewSnapshot;
    /* The review borrows archive until destruction. Create/Submit/Destroy belong
 * to its creating thread. Keep the task queue alive while submitted work runs.
 * Read and RowAt copy memory; neither touches the database or launches tests.
 * A completed comparison remains readable if Stop arrived just after capture.
 * ready, rather than task state alone, identifies a complete comparison.
 * Failed creation clears *out_review. Other read failures preserve outputs. */
    UmiStatus UmiTestArchiveReviewCreate(UmiTestArchive *archive, uint64_t baseline_id, uint64_t candidate_id,
                                         UmiTestArchiveReview **out_review);
    UmiStatus UmiTestArchiveReviewSubmit(UmiTestArchiveReview *review, UmiTaskQueue *queue);
    UmiStatus UmiTestArchiveReviewCancel(UmiTestArchiveReview *review);
    UmiStatus UmiTestArchiveReviewRead(UmiTestArchiveReview *review,
                                       UmiTestArchiveReviewSnapshot *out_snapshot);
    UmiStatus UmiTestArchiveReviewRowAt(UmiTestArchiveReview *review, size_t index,
                                        UmiTestArchiveComparisonRow *out_row);
    /* BUSY leaves a queued/running review alive. Request Stop and poll instead of
 * joining a worker from a GUI callback. NULL is accepted. */
    UmiStatus UmiTestArchiveReviewDestroy(UmiTestArchiveReview *review);
#ifdef __cplusplus
}
#endif
#endif
