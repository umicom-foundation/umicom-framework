/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/review.h
 *
 * PURPOSE:
 *   Own a retained quality report and publish only the active background request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CODEGUARD_REVIEW_H
#define UMICOM_CODEGUARD_REVIEW_H
#include "umicom/codeguard/service.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCodeGuardReview UmiCodeGuardReview;

/* Plain values. pending means the controller still owns a job awaiting poll or
 * retirement. Task state and operation_status describe execution; status is the
 * publication outcome and may be CANCELLED/INVALID_STATE after task success.
 * has_report and summary describe the retained report for the context passed
 * to Poll, which may predate the current attempt. */
typedef struct UmiCodeGuardReviewSnapshot {
    uint64_t request_id;
    uint64_t context_id;
    uint64_t report_context_id;
    UmiTaskState state;
    UmiStatus status;
    UmiStatus operation_status;
    int pending;
    int cancel_requested;
    int stale;
    int has_report;
    UmiCodeGuardSummary summary;
} UmiCodeGuardReviewSnapshot;

/* Require an absolute nonempty root; relative cwd changes must not redirect a
 * reusable review owner. A valid output is cleared on failure. No worker starts.
 * All operations, including result borrows and destruction, belong to this thread. */
UmiStatus UmiCodeGuardReviewCreate(const char *root, UmiCodeGuardReview **out_review);

/* Accept at most one job, copy profile (NULL selects defaults), and retain the
 * nonzero host context token. The host changes that token when the workspace,
 * trust or relevant request context changes, and must not reuse tokens for
 * different sessions. Success writes a monotonically increasing request ID.
 * Failure preserves the output and prior report. Poll retires the previous job
 * before another Begin; even a terminal unpolled job returns BUSY. */
UmiStatus UmiCodeGuardReviewBegin(UmiCodeGuardReview *review, UmiTaskQueue *queue,
    const UmiCodeGuardProfile *profile, uint64_t context_id, uint64_t *out_request_id);

/* Poll from the owner event loop. A mismatched context (including zero for a
 * closed workspace) permanently marks this attempt stale and requests cancellation.
 * Completed success is adopted only when neither stale nor cancelled. Poll
 * returns OK when it can report an operation failure in snapshot.status; an API
 * error preserves *out. It does not wait for analysis or join a worker.
 * A stale attempt has INVALID_STATE publication status while operation_status
 * retains the worker outcome. Cancel preserves a preceding operation failure. */
UmiStatus UmiCodeGuardReviewPoll(UmiCodeGuardReview *review, uint64_t context_id,
    UmiCodeGuardReviewSnapshot *out);

/* Request cancellation and suppress publication even if execution has just
 * succeeded but has not been polled. With no pending job this is an OK no-op.
 * A request never frees running work or invalidates the previous report. */
UmiStatus UmiCodeGuardReviewCancel(UmiCodeGuardReview *review);

/* Borrow only a report published for this nonzero context. NULL means no report,
 * wrong context or wrong owner thread. Finding borrows survive failed attempts,
 * and expire on successful replacement or destruction. Never free this result. */
const UmiCodeGuardResult *UmiCodeGuardReviewResult(const UmiCodeGuardReview *review,
    uint64_t context_id);

/* No blocking waits. BUSY preserves a queued/running owner; cancel and poll, or
 * stop/join the host queue before retrying. Terminal unpolled findings may be
 * discarded during destruction. A NULL owned review is an OK no-op. */
UmiStatus UmiCodeGuardReviewDestroy(UmiCodeGuardReview **in_out_review);

#ifdef __cplusplus
}
#endif
#endif
