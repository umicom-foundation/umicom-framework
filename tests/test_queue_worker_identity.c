/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_queue_worker_identity.c
 *
 * PURPOSE:
 *   Check queue worker identity without changing admission or shutdown state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/platform/task_queue.h"
#include <assert.h>

typedef struct IdentityWork {
    UmiTaskQueue *own;
    UmiTaskQueue *other;
    unsigned observations;
} IdentityWork;

static UmiStatus InspectIdentity(UmiTaskContext *context, void *data)
{
    IdentityWork *work = data;
    (void)context;
    assert(UmiTaskQueueIsCurrentWorker(work->own));
    assert(!UmiTaskQueueIsCurrentWorker(work->other));
    assert(!UmiTaskQueueIsCurrentWorker(NULL));
    UmiTaskQueueShutdownSnapshot snapshot;
    assert(UmiTaskQueueCaptureShutdown(work->own, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.phase == UMI_TASK_QUEUE_ACCEPTING);
    ++work->observations;
    return UMI_STATUS_OK;
}

int main(void)
{
    IdentityWork work = {0};
    UmiTaskQueueConfig config = {1U, 2U};
    UmiTask *task = NULL;
    assert(!UmiTaskQueueIsCurrentWorker(NULL));
    assert(umi_task_queue_create(&config, &work.own) == UMI_STATUS_OK);
    assert(umi_task_queue_create(&config, &work.other) == UMI_STATUS_OK);
    assert(!UmiTaskQueueIsCurrentWorker(work.own));
    UmiTaskConfig item = {"Inspect worker identity", InspectIdentity, &work, NULL, NULL};
    assert(umi_task_create(&item, &task) == UMI_STATUS_OK);
    assert(umi_task_queue_submit(work.own, task) == UMI_STATUS_OK);
    assert(umi_task_wait(task, 5000U) == UMI_STATUS_OK);
    assert(umi_task_result(task) == UMI_STATUS_OK);
    assert(umi_task_queue_shutdown(work.own, 0) == UMI_STATUS_OK);
    assert(work.observations == 1U);
    assert(!UmiTaskQueueIsCurrentWorker(work.own));
    assert(UmiTaskQueueReleaseStopped(&work.own) == UMI_STATUS_OK);
    assert(umi_task_queue_shutdown(work.other, 0) == UMI_STATUS_OK);
    assert(UmiTaskQueueReleaseStopped(&work.other) == UMI_STATUS_OK);
    umi_task_destroy(task);
    return 0;
}
