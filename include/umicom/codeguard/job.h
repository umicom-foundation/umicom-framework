/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/job.h
 *
 * PURPOSE:
 *   Own one background source scan and transfer only complete, uncancelled findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CODEGUARD_JOB_H
#define UMICOM_CODEGUARD_JOB_H
#include "umicom/codeguard/profile.h"
#include "umicom/codeguard/summary.h"
#include "umicom/platform/task_queue.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CODEGUARD_JOB_ROOT_CAPACITY 1024U
typedef struct UmiCodeGuardJob UmiCodeGuardJob;

/* Plain values; no borrowed finding or worker pointer crosses this boundary.
 * status is BUSY until terminal. operation_status preserves the worker's
 * original failure even when the task state is CANCELLED. If cancellation
 * prevents dispatch, operation_completed is false and operation_status BUSY.
 * Summary is exposed only after successful completion and remains available
 * after Take; result_available then becomes false. */
typedef struct UmiCodeGuardJobSnapshot {
    UmiTaskState state;
    UmiStatus status;
    UmiStatus operation_status;
    int operation_completed;
    int result_available;
    UmiCodeGuardSummary summary;
} UmiCodeGuardJobSnapshot;

/* Copy the profile (NULL selects defaults) and resolve root to an absolute
 * lexical path now, before dispatch. Reject an empty or unrepresentable root;
 * do not silently truncate it. No scan or thread is started. A non-NULL output
 * is cleared on failure. The resolved root fits 1023 UTF-8 bytes.
 * Create, Submit, Take and Destroy belong to the creating thread. */
UmiStatus UmiCodeGuardJobCreate(const char *root, const UmiCodeGuardProfile *profile,
    UmiCodeGuardJob **out_job);

/* Submit once to an existing shared queue. Full/stopping queue rejection
 * leaves a CREATED job reusable; the queue retains its own task reference.
 * The job must stay alive while queued/running, including its copied input. */
UmiStatus UmiCodeGuardJobSubmit(UmiCodeGuardJob *job, UmiTaskQueue *queue);

/* Cancellation and snapshot reads may run from another thread while it holds
 * a live job borrow. They must not race destruction. Cancel never waits for
 * analysis or changes an already terminal task. It cannot interrupt OS I/O. */
UmiStatus UmiCodeGuardJobCancel(UmiCodeGuardJob *job);
UmiStatus UmiCodeGuardJobRead(UmiCodeGuardJob *job, UmiCodeGuardJobSnapshot *out);

/* Transfer the completed result exactly once. Failure/BUSY preserves *out.
 * Cancelled/failed candidates are never transferred. The owner releases the
 * successful result with umi_codeguard_result_destroy, or swaps it into its
 * own retained result. Job destruction does not release a transferred result. */
UmiStatus UmiCodeGuardJobTake(UmiCodeGuardJob *job, UmiCodeGuardResult **out);

/* Release a CREATED or terminal job and clear the owned pointer. BUSY leaves
 * queued/running work alive; cancel and poll first. No joins or forced exits.
 * A NULL owned job is an OK no-op. Exclude all concurrent readers/cancellers
 * before destruction; this is not a reference-counted job lifetime. */
UmiStatus UmiCodeGuardJobDestroy(UmiCodeGuardJob **in_out_job);

/* Copy the immutable absolute root, including its terminator. Insufficient
 * capacity preserves the complete destination. A live borrow is required;
 * neither operation may race destruction. No scan or worker is scheduled. */
UmiStatus UmiCodeGuardJobCopyRoot(const UmiCodeGuardJob *job, char *out, size_t capacity);

/* Resolve root now using the same policy as Create, then compare lexical paths.
 * Success writes 0 or 1; failure preserves *out_match. This does not resolve
 * links, detect directory replacement, or enforce a filesystem sandbox. */
UmiStatus UmiCodeGuardJobMatchRoot(const UmiCodeGuardJob *job,
    const char *root, int *out_match);

#ifdef __cplusplus
}
#endif
#endif
