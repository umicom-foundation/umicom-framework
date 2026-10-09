/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_job.c
 * PURPOSE: Exercise copied requests, cancellation and owner-thread result transfer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/threading.h"
#include "umicom/vcs/working_tree_job.h"
#include <stdatomic.h>

static atomic_int entered, released, calls;
static int fail_read;
/* Hold the observation at a known boundary, so cancellation and ownership checks do not race a fast
 * Git. */
UmiStatus UmiVcsWorkingTreeRead(const UmiVcsWorkingTreeRequest *request, UmiVcsWorkingTree **out)
{
    static const char payload[] = HEADERS CHILD "framework\0";
    CHECK(strcmp(request->repository_root, "copied root") == 0);
    CHECK(strcmp(request->git_program, "copied git") == 0);
    CHECK(request->timeout_ms == 777U && request->cancellation != NULL);
    atomic_fetch_add(&calls, 1);
    atomic_store(&entered, 1);
    while (!atomic_load(&released))
    {
        if (umi_cancellation_token_is_requested(request->cancellation))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    if (fail_read)
        return UMI_STATUS_IO_ERROR;
    return UmiVcsWorkingTreeParse(payload, sizeof(payload) - 1U, out);
}

/* Keep every wait bounded so a regression becomes a clear test failure rather than a hung suite. */
static UmiVcsWorkingTreeJobSnapshot Await(UmiVcsWorkingTreeJob *job)
{
    UmiVcsWorkingTreeJobSnapshot snapshot;
    for (unsigned index = 0U; index < 5000U; ++index)
    {
        CHECK(UmiVcsWorkingTreeJobRead(job, &snapshot) == UMI_STATUS_OK);
        if (snapshot.state == UMI_TASK_SUCCEEDED || snapshot.state == UMI_TASK_FAILED ||
            snapshot.state == UMI_TASK_CANCELLED)
            return snapshot;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
    return snapshot;
}
typedef struct WrongOwner
{
    UmiVcsWorkingTreeJob *job;
    UmiTaskQueue *queue;
} WrongOwner;
/* Scheduling and transferring ownership are owner-thread operations, unlike cancellation and
 * polling. */
static int OtherThread(void *data)
{
    WrongOwner *owner = data;
    UmiVcsWorkingTree *tree = NULL;
    CHECK(UmiVcsWorkingTreeJobSubmit(owner->job, owner->queue) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiVcsWorkingTreeJobTake(owner->job, &tree) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiVcsWorkingTreeJobDestroy(owner->job) == UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[] = "copied root", program[] = "copied git";
    UmiVcsWorkingTreeRequest request = {root, program, 777U, NULL};
    UmiVcsWorkingTreeJob *job = NULL;
    UmiVcsWorkingTree *result = NULL;
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueConfig config = {1U, 2U};
    atomic_init(&entered, 0);
    atomic_init(&released, 0);
    atomic_init(&calls, 0);
    CHECK(UmiVcsWorkingTreeJobCreate(&request, &job) == UMI_STATUS_OK);
    memset(root, 'z', sizeof(root) - 1U);
    memset(program, 'z', sizeof(program) - 1U);
    UmiVcsWorkingTreeJobSnapshot snapshot;
    CHECK(UmiVcsWorkingTreeJobRead(job, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_TASK_CREATED && snapshot.status == UMI_STATUS_BUSY);
    CHECK(UmiVcsWorkingTreeJobTake(job, &result) == UMI_STATUS_BUSY && result == NULL);
    if (strcmp(mode, "created") == 0)
    {
        CHECK(atomic_load(&calls) == 0);
    }
    else if (strcmp(mode, "cancel-created") == 0)
    {
        CHECK(UmiVcsWorkingTreeJobCancel(job) == UMI_STATUS_OK);
        snapshot = Await(job);
        CHECK(snapshot.state == UMI_TASK_CANCELLED);
        CHECK(UmiVcsWorkingTreeJobTake(job, &result) == UMI_STATUS_CANCELLED);
        CHECK(atomic_load(&calls) == 0);
    }
    else
    {
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
        if (strcmp(mode, "wrong-owner") == 0)
        {
            WrongOwner owner = {job, queue};
            UmiThread *thread = NULL;
            CHECK(umi_thread_start(OtherThread, &owner, &thread) == UMI_STATUS_OK);
            CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_OK);
            umi_thread_destroy(thread);
        }
        else
        {
            fail_read = strcmp(mode, "read-failure") == 0;
            CHECK(UmiVcsWorkingTreeJobSubmit(job, queue) == UMI_STATUS_OK);
            for (unsigned index = 0U; index < 5000U && !atomic_load(&entered); ++index)
                umi_thread_sleep_ms(1U);
            CHECK(atomic_load(&entered));
            CHECK(UmiVcsWorkingTreeJobSubmit(job, queue) != UMI_STATUS_OK);
            CHECK(UmiVcsWorkingTreeJobDestroy(job) == UMI_STATUS_BUSY);
            CHECK(UmiVcsWorkingTreeJobTake(job, &result) == UMI_STATUS_BUSY);
            if (strcmp(mode, "cancel-running") == 0)
                CHECK(UmiVcsWorkingTreeJobCancel(job) == UMI_STATUS_OK);
            else
                atomic_store(&released, 1);
            snapshot = Await(job);
            UmiStatus expected = strcmp(mode, "cancel-running") == 0 ? UMI_STATUS_CANCELLED
                                 : fail_read                         ? UMI_STATUS_IO_ERROR
                                                                     : UMI_STATUS_OK;
            CHECK(UmiVcsWorkingTreeJobTake(job, &result) == expected);
            if (expected == UMI_STATUS_OK)
            {
                CHECK(result != NULL);
                CHECK(UmiVcsWorkingTreeJobTake(job, &result) == UMI_STATUS_NOT_FOUND);
                if (strcmp(mode, "late-stop") == 0)
                    CHECK(UmiVcsWorkingTreeJobCancel(job) == UMI_STATUS_OK);
            }
            else
                CHECK(result == NULL);
            CHECK(strcmp(mode, "success") == 0 || strcmp(mode, "late-stop") == 0 ||
                  strcmp(mode, "cancel-running") == 0 || strcmp(mode, "read-failure") == 0);
        }
        umi_task_queue_destroy(queue);
    }
    CHECK(UmiVcsWorkingTreeJobDestroy(job) == UMI_STATUS_OK);
    UmiVcsWorkingTreeDestroy(result);
    return 0;
}
