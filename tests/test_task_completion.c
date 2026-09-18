/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_task_completion.c
 * PURPOSE:
 *   Hold a worker immediately after it publishes completion, then release its
 *   task from the waiter to verify that returning never rereads freed storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #value); \
    return EXIT_FAILURE; } } while (0)

/* Linker wrapping is used only in this Linux test, never in production. */
static _Thread_local int pauseAtCompletion;
static atomic_int completionPublished;
static atomic_int releaseWorker;
UmiStatus __real_umi_mutex_unlock(UmiMutex *mutex);
UmiStatus __wrap_umi_mutex_unlock(UmiMutex *mutex);

UmiStatus __wrap_umi_mutex_unlock(UmiMutex *mutex)
{
    UmiStatus status = __real_umi_mutex_unlock(mutex);
    if (pauseAtCompletion) {
        pauseAtCompletion = 0;
        atomic_store(&completionPublished, 1);
        while (!atomic_load(&releaseWorker)) umi_thread_sleep_ms(1U);
    }
    return status;
}

static UmiStatus Work(UmiTaskContext *context, void *userData)
{
    (void)context;
    (void)userData;
    /* The next unlock in this thread publishes the completed task. */
    pauseAtCompletion = 1;
    return UMI_STATUS_OK;
}

static int Run(void *argument)
{
    UmiStatus status = umi_task_run(argument);
    return status == UMI_STATUS_OK ? 0 : 1;
}

int main(void)
{
    UmiTaskConfig config = {0};
    UmiTask *task = NULL;
    UmiThread *worker = NULL;
    int result = -1;
    config.label = "Completion ownership";
    config.function = Work;
    CHECK(umi_task_create(&config, &task) == UMI_STATUS_OK);
    CHECK(umi_thread_start(Run, task, &worker) == UMI_STATUS_OK);
    for (unsigned attempt = 0U; attempt < 5000U &&
            !atomic_load(&completionPublished); ++attempt)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&completionPublished));
    CHECK(umi_task_wait(task, 5000U) == UMI_STATUS_OK);
    CHECK(umi_task_result(task) == UMI_STATUS_OK);
    umi_task_destroy(task);
    atomic_store(&releaseWorker, 1);
    CHECK(umi_thread_join(worker, &result) == UMI_STATUS_OK);
    umi_thread_destroy(worker);
    CHECK(result == 0);
    return EXIT_SUCCESS;
}
