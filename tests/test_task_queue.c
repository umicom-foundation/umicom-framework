/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_task_queue.c
 *
 * PURPOSE:
 *   Verify task execution, progress, cancellation, bounded queue statistics,
 *   idle waiting and deterministic worker shutdown.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>

typedef struct TaskFixture {
    atomic_int completed;
    atomic_uint progress_events;
} TaskFixture;

/*
 * Exercise progress sink and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void progress_sink(uint64_t task_id,
                          unsigned progress,
                          const char *message,
                          void *user_data)
{
    TaskFixture *fixture = (TaskFixture *)user_data;
    (void)task_id;
    (void)progress;
    (void)message;
    (void)atomic_fetch_add(&fixture->progress_events, 1U);
}

/*
 * Exercise counting task and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus counting_task(UmiTaskContext *context, void *user_data)
{
    TaskFixture *fixture = (TaskFixture *)user_data;
    assert(umi_task_context_report(context, 50U, "half") == UMI_STATUS_OK);
    (void)atomic_fetch_add(&fixture->completed, 1);
    return umi_task_context_report(context, 100U, "complete");
}

/*
 * Exercise cancellable task and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus cancellable_task(UmiTaskContext *context, void *user_data)
{
    unsigned index;
    (void)user_data;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 100U; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_task_context_is_cancelled(context)) {
            return UMI_STATUS_CANCELLED;
        }
        umi_thread_sleep_ms(2U);
    }
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTaskQueueConfig queue_config = {2U, 16U};
    UmiTaskQueue *queue = NULL;
    UmiTask *tasks[6] = {0};
    UmiTask *cancel_task = NULL;
    TaskFixture fixture;
    size_t index;
    UmiTaskQueueStats stats;

    atomic_init(&fixture.completed, 0);
    atomic_init(&fixture.progress_events, 0U);

    assert(umi_task_queue_create(&queue_config, &queue) == UMI_STATUS_OK);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 6U; ++index) {
        UmiTaskConfig config = {
            "counting task",
            counting_task,
            &fixture,
            progress_sink,
            &fixture
        };
        assert(umi_task_create(&config, &tasks[index]) == UMI_STATUS_OK);
        assert(umi_task_queue_submit(queue, tasks[index]) == UMI_STATUS_OK);
    }

    {
        UmiTaskConfig config = {
            "cancellable task",
            cancellable_task,
            NULL,
            NULL,
            NULL
        };
        assert(umi_task_create(&config, &cancel_task) == UMI_STATUS_OK);
        assert(umi_task_queue_submit(queue, cancel_task) == UMI_STATUS_OK);
        assert(umi_task_cancel(cancel_task) == UMI_STATUS_OK);
    }

    assert(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 6U; ++index) {
        assert(umi_task_wait(tasks[index], 1000U) == UMI_STATUS_OK);
        assert(umi_task_state(tasks[index]) == UMI_TASK_SUCCEEDED);
        assert(umi_task_result(tasks[index]) == UMI_STATUS_OK);
        assert(umi_task_progress(tasks[index]) == 100U);
    }
    assert(umi_task_wait(cancel_task, 1000U) == UMI_STATUS_OK);
    assert(umi_task_state(cancel_task) == UMI_TASK_CANCELLED);

    stats = umi_task_queue_stats(queue);
    assert(stats.submitted == 7U);
    assert(stats.completed == 7U);
    assert(stats.failed == 0U);
    assert(stats.cancelled >= 1U);
    assert(atomic_load(&fixture.completed) == 6);
    assert(atomic_load(&fixture.progress_events) == 12U);

    assert(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 6U; ++index) {
        umi_task_destroy(tasks[index]);
    }
    umi_task_destroy(cancel_task);
    umi_task_queue_destroy(queue);

    (void)printf("task queue contract passed\n");
    return 0;
}
