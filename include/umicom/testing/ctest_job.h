/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_job.h
 *
 * PURPOSE:
 *   Run copied CTest selections on the Framework task queue while exposing
 *   completed results and cooperative cancellation to an application owner.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TESTING_CTEST_JOB_H
#define UMICOM_TESTING_CTEST_JOB_H

#include "umicom/platform/task_queue.h"
#include "umicom/testing/ctest_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CTEST_JOB_PATH_CAPACITY 1024U
#define UMI_CTEST_JOB_CONFIGURATION_CAPACITY 128U
#define UMI_CTEST_JOB_MAX_ATTEMPTS 4096U

/** One copied, authorised discovery origin. working_directory is deliberately
 * absent: build_directory is the CTest catalogue root, not a test's cwd.
 * timeout_ms bounds the complete CTest invocation, including fixtures; zero
 * leaves individual test deadlines to CTest. enabled is exactly zero or one.
 * Initialise every byte before assigning fields. Strings must be terminated.
 */
typedef struct UmiCtestJobRequest {
    char test_id[UMI_TEST_ID_CAPACITY];
    char name[UMI_TEST_NAME_CAPACITY];
    char build_directory[UMI_CTEST_JOB_PATH_CAPACITY];
    char configuration[UMI_CTEST_JOB_CONFIGURATION_CAPACITY];
    uint32_t timeout_ms;
    int enabled;
} UmiCtestJobRequest;

/** Zero repeat_count means one pass. Attempts run in selection order, once
 * per pass. stop_on_failure stops after FAILED, NOT_RUN, TIMED_OUT, or a
 * reporting/process error; a legitimate SKIPPED result does not stop a run.
 */
typedef struct UmiCtestJobOptions {
    uint32_t repeat_count;
    int stop_on_failure;
} UmiCtestJobOptions;

typedef struct UmiCtestJob UmiCtestJob;

/** completed counts recorded attempts, including skipped or not-run attempts.
 * While running, planned - completed includes the active unfinished attempt.
 * Once terminal, that difference counts attempts which never started.
 * state uses the existing task states. status is the task result when terminal;
 * first_error retains an earlier process/report error even after cancellation.
 * Returned strings and counters are copies, safe to use after the call.
 */
typedef struct UmiCtestJobSnapshot {
    uint64_t task_id;
    UmiTaskState state;
    UmiStatus status;
    UmiStatus first_error;
    size_t planned;
    size_t completed;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t cancelled;
    size_t timed_out;
    size_t not_run;
    uint64_t duration_ms;
    uint32_t active_attempt;
    char active_test_id[UMI_TEST_ID_CAPACITY];
    char active_name[UMI_TEST_NAME_CAPACITY];
} UmiCtestJobSnapshot;

/** Validate the whole selection and allocate result storage before any launch.
 * Duplicate IDs and over-capacity plans are rejected. Caller requests/options
 * are copied and may immediately be freed. The job owns no GTK objects or live
 * application registries. Creation and submission do not grant workspace trust;
 * the application must authorise every selected build root first.
 * Returns a CREATED job. Results use up to roughly 130 MiB at the hard limit.
 */
UmiStatus UmiCtestJobCreate(const UmiCtestJobRequest *requests, size_t count,
    const UmiCtestJobOptions *options, UmiCtestJob **outJob);

/** Create the same owned selection with an explicit absolute tool folder.
 * Copy the directory before returning; NULL/empty keeps inherited PATH.
 * This option applies to every attempt without changing archived test identities
 * or the discovery build root. No process starts during creation. */
UmiStatus UmiCtestJobCreateWithToolDirectory(const UmiCtestJobRequest *requests,
    size_t count, const UmiCtestJobOptions *options, const char *directory,
    UmiCtestJob **outJob);

/** Submit once. A queue-full rejection leaves a CREATED job available to retry.
 * The queue must remain alive until its owner drains/shuts it down. Create,
 * Submit and Destroy belong to the creating thread; none joins a worker.
 */
UmiStatus UmiCtestJobSubmit(UmiCtestJob *job, UmiTaskQueue *queue);

/** Request Stop without waiting. The canonical task token reaches the active
 * UmiCtestRunConfigured call; later attempts are not launched. Completed results
 * remain readable. Cancellation is not a promise to terminate descendant
 * processes or services created independently by a test. A late Stop leaves
 * a terminal task and its verified results unchanged.
 */
UmiStatus UmiCtestJobCancel(UmiCtestJob *job);

/** Snapshot and ResultAt may be called concurrently with the worker. The owner
 * must keep the job alive through each call and serialise its destruction.
 * ResultAt returns only completely recorded rows; an unfinished index returns
 * NOT_FOUND and leaves output arguments unchanged. Attempts are one-based.
 */
UmiStatus UmiCtestJobGetSnapshot(UmiCtestJob *job, UmiCtestJobSnapshot *outSnapshot);
UmiStatus UmiCtestJobResultAt(UmiCtestJob *job, size_t index,
    UmiTestResult *outResult, uint32_t *outAttempt);

/** Release a CREATED or terminal job. BUSY leaves a queued/running job intact;
 * cancel it, continue polling, then destroy. Wrong-thread calls return
 * INVALID_STATE. A cancelled queued task may still be held by the queue, but
 * task ownership guarantees that its callback will not access this payload.
 * NULL is accepted. After success, the pointer is no longer usable.
 */
UmiStatus UmiCtestJobDestroy(UmiCtestJob *job);

#ifdef __cplusplus
}
#endif
#endif
