/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/queue_shutdown/client/lifecycle_cases.c
 *
 * PURPOSE:
 *   Exercise real installed queue ownership, cancellation and error contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "lifecycle_cases.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ConsumerWork {
    UmiTaskQueue *queue; /* Borrowed until the controller joins every worker. */
    atomic_int started;
    atomic_int release;
    atomic_int entered;
    atomic_int returned;
    UmiStatus result;
    int checkSelf;
} ConsumerWork;

static int ExpectStatus(UmiStatus actual, UmiStatus expected, const char *operation)
{
    if (actual == expected) return 1;
    fprintf(stderr, "%s: expected %s, received %s.\n",
        operation, umi_status_text(expected), umi_status_text(actual));
    return 0;
}

static UmiStatus Work(UmiTaskContext *context, void *data)
{
    ConsumerWork *work = data;
    atomic_fetch_add(&work->entered, 1);
    atomic_store(&work->started, 1);
    if (work->checkSelf) {
        /* A callback cannot wait for the queue which is waiting for it.
         * These refusals must remain available through the installed API. */
        if (!UmiTaskQueueIsCurrentWorker(work->queue) ||
            umi_task_queue_wait_idle(work->queue, 1U) != UMI_STATUS_INVALID_STATE ||
            umi_task_queue_shutdown(work->queue, 0) != UMI_STATUS_INVALID_STATE ||
            UmiTaskQueueTryFinishShutdown(work->queue) != UMI_STATUS_INVALID_STATE)
            return UMI_STATUS_INTERNAL_ERROR;
    }
    /* The fixture has its own finite escape path. Cancellation is cooperative;
     * no test depends on killing a thread to make shutdown finish. */
    for (unsigned attempt = 0U; attempt < 10000U; ++attempt) {
        if (umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
        if (atomic_load(&work->release)) {
            atomic_fetch_add(&work->returned, 1);
            return work->result;
        }
        umi_thread_sleep_ms(1U);
    }
    return UMI_STATUS_TIMEOUT;
}

static int WaitForEntry(ConsumerWork *work)
{
    for (unsigned attempt = 0U; attempt < 10000U; ++attempt) {
        if (atomic_load(&work->started)) return 1;
        umi_thread_sleep_ms(1U);
    }
    fputs("The worker did not enter its callback within the fixture budget.\n", stderr);
    return 0;
}

int UmiShutdownConsumerRunCase(const char *name)
{
    const int cancel = name != NULL && strcmp(name, "cancel-running-and-pending") == 0;
    const int rejected = name != NULL && strcmp(name, "rejected-reusable") == 0;
    const int callerRelease = name != NULL && strcmp(name, "caller-release") == 0;
    const int callbackError = name != NULL && strcmp(name, "callback-error") == 0;
    const int self = name != NULL && strcmp(name, "worker-self") == 0;
    const int drain = name != NULL && strcmp(name, "drain") == 0;
    if (!cancel && !rejected && !callerRelease && !callbackError && !self && !drain) {
        fputs("Unknown shutdown consumer scenario.\n", stderr);
        return 2;
    }

    UmiTaskQueue *queue = NULL;
    UmiTask *task = NULL;
    UmiTask *pending = NULL;
    ConsumerWork *work = calloc(1U, sizeof(*work));
    UmiTaskQueueConfig settings = {1U, 2U};
    UmiTaskQueueShutdownSnapshot snapshot;
    int failed = 1;
    if (work == NULL) return 1;
    atomic_init(&work->started, 0);
    atomic_init(&work->release, !cancel);
    atomic_init(&work->entered, 0);
    atomic_init(&work->returned, 0);
    work->result = callbackError ? UMI_STATUS_INTERNAL_ERROR : UMI_STATUS_OK;
    work->checkSelf = self;

    if (!ExpectStatus(umi_task_queue_create(&settings, &queue), UMI_STATUS_OK, "create queue"))
        goto cleanup;
    work->queue = queue;
    if (UmiTaskQueueIsCurrentWorker(queue) || UmiTaskQueueIsCurrentWorker(NULL)) goto cleanup;
    if (!ExpectStatus(UmiTaskQueueCaptureShutdown(queue, &snapshot), UMI_STATUS_OK, "capture accepting queue") ||
        snapshot.phase != UMI_TASK_QUEUE_ACCEPTING) goto cleanup;
    UmiTaskQueue *owned = queue;
    if (!ExpectStatus(UmiTaskQueueReleaseStopped(&queue), UMI_STATUS_BUSY, "refuse active queue release") ||
        queue != owned) goto cleanup;
    if (!ExpectStatus(UmiTaskQueueTryFinishShutdown(queue), UMI_STATUS_INVALID_STATE,
            "refuse joining an accepting queue")) goto cleanup;

    UmiTaskConfig config = {"Inspect an installed SDK task", Work, work, NULL, NULL};
    if (!ExpectStatus(umi_task_create(&config, &task), UMI_STATUS_OK, "create task")) goto cleanup;
    if (rejected) {
        if (!ExpectStatus(UmiTaskQueueRequestShutdown(queue, 0, 0), UMI_STATUS_OK, "close admission") ||
            !ExpectStatus(umi_task_queue_submit(queue, task), UMI_STATUS_INVALID_STATE, "reject new submission") ||
            umi_task_state(task) != UMI_TASK_CREATED) goto cleanup;
        /* Rejection must not consume or cancel the caller's task. Running it
         * directly is valid because this queue never accepted it. */
        if (!ExpectStatus(umi_task_run(task), UMI_STATUS_OK, "reuse rejected task")) goto cleanup;
    } else {
        if (!ExpectStatus(umi_task_queue_submit(queue, task), UMI_STATUS_OK, "submit task")) goto cleanup;
        if (callerRelease) {
            /* The queue retains the task, but not this callback payload.
             * Keep work alive through join even after releasing our task ref. */
            umi_task_destroy(task);
            task = NULL;
        }
        if (cancel) {
            if (!WaitForEntry(work)) goto cleanup;
            if (!ExpectStatus(umi_task_create(&config, &pending), UMI_STATUS_OK, "create pending task") ||
                !ExpectStatus(umi_task_queue_submit(queue, pending), UMI_STATUS_OK, "submit pending task"))
                goto cleanup;
        }
        if (!ExpectStatus(UmiTaskQueueRequestShutdown(queue, cancel, cancel),
                UMI_STATUS_OK, "request shutdown")) goto cleanup;
    }
    /* This command-line consumer may block while joining. GUI adapters should
     * instead use the existing nonblocking polling demonstration where supported. */
    if (!ExpectStatus(umi_task_queue_shutdown(queue, 0), UMI_STATUS_OK, "join workers") ||
        !ExpectStatus(UmiTaskQueueCaptureShutdown(queue, &snapshot), UMI_STATUS_OK, "capture stopped queue"))
        goto cleanup;
    if (snapshot.phase != UMI_TASK_QUEUE_STOPPED || snapshot.tasks.queued != 0U ||
        snapshot.tasks.running != 0U) goto cleanup;
    const uint64_t expectedSubmitted = rejected ? 0U : cancel ? 2U : 1U;
    if (snapshot.tasks.submitted != expectedSubmitted ||
        snapshot.tasks.completed != expectedSubmitted ||
        snapshot.tasks.cancelled != (cancel ? 2U : 0U) ||
        snapshot.tasks.failed != (callbackError ? 1U : 0U)) {
        fputs("Stopped queue counters differ from accepted work.\n", stderr);
        goto cleanup;
    }
    if (atomic_load(&work->entered) != 1 ||
        atomic_load(&work->returned) != (cancel ? 0 : 1)) goto cleanup;
    if (task != NULL) {
        const UmiStatus expectedResult = cancel ? UMI_STATUS_CANCELLED :
            callbackError ? UMI_STATUS_INTERNAL_ERROR : UMI_STATUS_OK;
        const UmiTaskState expectedState = cancel ? UMI_TASK_CANCELLED :
            callbackError ? UMI_TASK_FAILED : UMI_TASK_SUCCEEDED;
        if (!ExpectStatus(umi_task_result(task), expectedResult, "task outcome") ||
            umi_task_state(task) != expectedState) goto cleanup;
    }
    if (pending != NULL &&
        (umi_task_state(pending) != UMI_TASK_CANCELLED ||
         umi_task_result(pending) != UMI_STATUS_CANCELLED)) goto cleanup;
    if (!ExpectStatus(UmiTaskQueueReleaseStopped(&queue), UMI_STATUS_OK, "release joined queue") ||
        queue != NULL ||
        !ExpectStatus(UmiTaskQueueReleaseStopped(&queue), UMI_STATUS_OK, "repeat empty release"))
        goto cleanup;
    failed = 0;

cleanup:
    if (queue != NULL) {
        atomic_store(&work->release, 1);
        (void)UmiTaskQueueRequestShutdown(queue, 1, 1);
        if (!ExpectStatus(umi_task_queue_shutdown(queue, 1), UMI_STATUS_OK, "cleanup join") ||
            !ExpectStatus(UmiTaskQueueReleaseStopped(&queue), UMI_STATUS_OK, "cleanup release")) {
            /* On a native lifecycle failure, retain heap payloads and owner
             * references until this failed test process exits. Returning through
             * stack-backed callback data would create a use-after-return. */
            fputs("Queue cleanup failed; live owners were retained for process exit.\n", stderr);
            return 1;
        }
    }
    umi_task_destroy(pending);
    umi_task_destroy(task);
    free(work);
    if (!failed) printf("Installed shutdown contract: %s passed.\n", name);
    return failed;
}
