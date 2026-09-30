/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/queue_shutdown/test_faults.c
 * PURPOSE:
 *   Linker wrappers inject failures at the public/native boundary. The real queue, thread
 *   record and operating-system threads are used throughout.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Linker wrappers inject failures at the public/native boundary. The real queue,
 * thread record and operating-system threads are used throughout. */
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); exit(1); } } while (0)
UmiStatus __real_UmiThreadTryJoin(UmiThread *thread);
UmiStatus __real_UmiThreadRelease(UmiThread **thread);
UmiStatus __real_umi_thread_join(UmiThread *thread, int *result);
static unsigned tryCalls, releaseCalls, blockingCalls;
static unsigned failTry, busyTry, failRelease;
static uintptr_t released[64];
static unsigned releasedCount;
UmiStatus __wrap_UmiThreadTryJoin(UmiThread *thread)
{
    ++tryCalls;
    if (tryCalls == failTry) return UMI_STATUS_INTERNAL_ERROR;
    if (tryCalls == busyTry) return UMI_STATUS_BUSY;
    return __real_UmiThreadTryJoin(thread);
}
UmiStatus __wrap_UmiThreadRelease(UmiThread **thread)
{
    uintptr_t address = thread != NULL ? (uintptr_t)*thread : 0U;
    if (address != 0U) {
        ++releaseCalls;
        if (releaseCalls == failRelease) return UMI_STATUS_IO_ERROR;
        for (unsigned i = 0U; i < releasedCount; ++i) CHECK(released[i] != address);
    }
    UmiStatus status = __real_UmiThreadRelease(thread);
    if (status == UMI_STATUS_OK && address != 0U) {
        CHECK(releasedCount < 64U);
        released[releasedCount++] = address;
    }
    return status;
}
UmiStatus __wrap_umi_thread_join(UmiThread *thread, int *result)
{
    ++blockingCalls;
    return __real_umi_thread_join(thread, result);
}
static void Finish(UmiTaskQueue *queue)
{
    UmiStatus status = UMI_STATUS_BUSY;
    for (unsigned i = 0U; i < 5000U && status == UMI_STATUS_BUSY; ++i) {
        status = UmiTaskQueueTryFinishShutdown(queue);
        if (status == UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
    }
    CHECK(status == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    UmiTaskQueueConfig config = {4U, 8U};
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueShutdownSnapshot snapshot;
    if (argc != 2) return 2;
    CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    UmiTaskQueue *same = queue;
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "join_failure") || !strcmp(argv[1], "legacy_retry")) {
        failTry = 1U;
        CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_INTERNAL_ERROR);
        CHECK(UmiTaskQueueCaptureShutdown(queue, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.phase == UMI_TASK_QUEUE_STOP_REQUESTED);
        CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_BUSY && queue == same);
        if (!strcmp(argv[1], "legacy_retry")) {
            CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
            CHECK(blockingCalls == 4U);
        } else Finish(queue);
    } else if (!strcmp(argv[1], "busy_progress")) {
        busyTry = 1U;
        CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_BUSY);
        CHECK(tryCalls == 4U); /* Visit later slots even if the first is busy. */
        Finish(queue);
    } else if (!strcmp(argv[1], "release_failure") || !strcmp(argv[1], "no_blocking_join")) Finish(queue);
    else return 2;
    if (!strcmp(argv[1], "release_failure")) {
        failRelease = 2U;
        CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_IO_ERROR && queue == same);
        CHECK(releasedCount == 1U);
        CHECK(UmiTaskQueueCaptureShutdown(queue, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.phase == UMI_TASK_QUEUE_STOPPED);
    }
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK && queue == NULL);
    CHECK(releasedCount == 4U);
    if (strcmp(argv[1], "legacy_retry")) CHECK(blockingCalls == 0U);
    printf("PASS %s: %u native owners released once\n", argv[1], releasedCount);
    return 0;
}
