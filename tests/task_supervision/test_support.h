/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/test_support.h
 * PURPOSE: Deterministic gates and assertions for task ownership regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TASK_SUPERVISION_TEST_SUPPORT_H
#define UMICOM_TASK_SUPERVISION_TEST_SUPPORT_H
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/clock.h"
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    exit(EXIT_FAILURE); } } while (0)

typedef struct TestGate {
    atomic_int entered;
    atomic_int released;
    atomic_int exited;
    atomic_int cancelled;
    atomic_uint calls;
    int honourCancel;
    UmiStatus returnStatus;
} TestGate;

static inline uint64_t TestNowMs(void)
{
    UmiClock clock = umi_clock_system();
    return clock.monotonic_nanoseconds(&clock) / 1000000ULL;
}
static inline void TestAwait(const atomic_int *flag)
{
    uint64_t start = TestNowMs();
    while (!atomic_load(flag)) {
        CHECK(TestNowMs() - start < 10000U);
        umi_thread_sleep_ms(1U);
    }
}
static inline void TestGateInit(TestGate *gate, int honourCancel)
{
    atomic_init(&gate->entered, 0);
    atomic_init(&gate->released, 0);
    atomic_init(&gate->exited, 0);
    atomic_init(&gate->cancelled, 0);
    atomic_init(&gate->calls, 0U);
    gate->honourCancel = honourCancel;
    gate->returnStatus = UMI_STATUS_OK;
}
static inline UmiStatus TestGatedTask(UmiTaskContext *context, void *payload)
{
    TestGate *gate = payload;
    CHECK(context != NULL);
    CHECK(UmiTaskContextCancellation(context) != NULL);
    CHECK(umi_task_context_id(context) != 0U);
    atomic_fetch_add(&gate->calls, 1U);
    atomic_store(&gate->entered, 1);
    while (!atomic_load(&gate->released)) {
        if (umi_task_context_is_cancelled(context)) {
            CHECK(umi_cancellation_token_is_requested(UmiTaskContextCancellation(context)));
            atomic_store(&gate->cancelled, 1);
            if (gate->honourCancel) break;
        }
        umi_thread_sleep_ms(1U);
    }
    atomic_store(&gate->exited, 1);
    return gate->returnStatus;
}
static inline UmiTask *TestTask(TestGate *gate)
{
    UmiTaskConfig config = {0};
    UmiTask *task = NULL;
    config.label = "Umicom Notes: index project";
    config.function = TestGatedTask;
    config.user_data = gate;
    CHECK(umi_task_create(&config, &task) == UMI_STATUS_OK);
    return task;
}
static inline UmiTaskQueue *TestQueue(size_t workers, size_t capacity)
{
    UmiTaskQueueConfig config = {workers, capacity};
    UmiTaskQueue *queue = NULL;
    CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    return queue;
}
static inline void TestFinish(UmiTaskQueue *queue)
{
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    umi_task_queue_destroy(queue);
}
#endif
