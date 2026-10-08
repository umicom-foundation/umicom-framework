/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_review.c
 * PURPOSE: Verify queued comparison lifetime, cancellation and immutable publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/testing/archive_review.h"
typedef struct Gate
{
    atomic_int entered, release;
} Gate;
static UmiStatus WaitGate(UmiTaskContext *context, void *data)
{
    (void)context;
    Gate *gate = data;
    atomic_store(&gate->entered, 1);
    while (!atomic_load(&gate->release))
        umi_thread_sleep_ms(1);
    return UMI_STATUS_OK;
}
static void Await(UmiTestArchiveReview *review, UmiTestArchiveReviewSnapshot *state)
{
    for (unsigned i = 0; i < 5000U; ++i)
    {
        CHECK(UmiTestArchiveReviewRead(review, state) == UMI_STATUS_OK);
        if (state->state == UMI_TASK_SUCCEEDED || state->state == UMI_TASK_FAILED ||
            state->state == UMI_TASK_CANCELLED)
            return;
        umi_thread_sleep_ms(1);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(false);
    UmiTestArchiveEntry entry = {0};
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK);
    UmiTestArchiveReview *review = NULL;
    CHECK(UmiTestArchiveReviewCreate(archive, entry.id, strcmp(name, "missing") == 0 ? 99 : entry.id,
                                     &review) == UMI_STATUS_OK);
    UmiTestArchiveComparisonRow row = {0};
    CHECK(UmiTestArchiveReviewRowAt(review, 0, &row) == UMI_STATUS_NOT_FOUND);
    if (strcmp(name, "created") == 0)
    {
        CHECK(UmiTestArchiveReviewDestroy(review) == UMI_STATUS_OK);
    }
    else
    {
        UmiTaskQueue *queue = NULL;
        UmiTaskQueueConfig config = umi_task_queue_config_default();
        config.worker_count = 1;
        config.capacity = 4;
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
        Gate gate;
        atomic_init(&gate.entered, 0);
        atomic_init(&gate.release, 0);
        UmiTaskConfig task_config = {0};
        task_config.function = WaitGate;
        task_config.user_data = &gate;
        UmiTask *blocker = NULL;
        CHECK(umi_task_create(&task_config, &blocker) == UMI_STATUS_OK);
        CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
        for (unsigned i = 0; i < 5000U && !atomic_load(&gate.entered); ++i)
            umi_thread_sleep_ms(1);
        CHECK(atomic_load(&gate.entered));
        CHECK(UmiTestArchiveReviewSubmit(review, queue) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveReviewDestroy(review) == UMI_STATUS_BUSY);
        CHECK(UmiTestArchiveReviewSubmit(review, queue) != UMI_STATUS_OK);
        if (strcmp(name, "cancel") == 0)
            CHECK(UmiTestArchiveReviewCancel(review) == UMI_STATUS_OK);
        atomic_store(&gate.release, 1);
        UmiTestArchiveReviewSnapshot state = {0};
        Await(review, &state);
        if (strcmp(name, "cancel") == 0)
            CHECK(!state.ready && state.status == UMI_STATUS_CANCELLED);
        else if (strcmp(name, "missing") == 0)
            CHECK(!state.ready && state.status == UMI_STATUS_NOT_FOUND);
        else
        {
            CHECK(strcmp(name, "success") == 0 || strcmp(name, "late-stop") == 0 ||
                  strcmp(name, "lifetime") == 0);
            CHECK(state.ready && state.status == UMI_STATUS_OK && state.summary.row_count == 2);
            CHECK(UmiTestArchiveReviewRowAt(review, 0, &row) == UMI_STATUS_OK);
            if (strcmp(name, "late-stop") == 0)
            {
                CHECK(UmiTestArchiveReviewCancel(review) == UMI_STATUS_OK);
                CHECK(UmiTestArchiveReviewRead(review, &state) == UMI_STATUS_OK && state.ready);
            }
        }
        CHECK(UmiTestArchiveReviewDestroy(review) == UMI_STATUS_OK);
        umi_task_queue_destroy(queue);
        umi_task_destroy(blocker);
    }
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}
