/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_removal.c
 * PURPOSE: Check queued removal ownership, contention and committed cancellation outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/testing/archive_removal.h"
typedef struct RemovalGate
{
    atomic_int entered, released;
} RemovalGate;
static UmiStatus HoldRemovalQueue(UmiTaskContext *context, void *data)
{
    (void)context;
    RemovalGate *gate = data;
    atomic_store(&gate->entered, 1);
    while (!atomic_load(&gate->released))
        umi_thread_sleep_ms(1);
    return UMI_STATUS_OK;
}
static void AwaitRemoval(UmiTestArchiveRemoval *removal, UmiTestArchiveRemovalSnapshot *out)
{
    for (unsigned i = 0; i < 5000; ++i)
    {
        CHECK(UmiTestArchiveRemovalRead(removal, out) == UMI_STATUS_OK);
        if (out->state == UMI_TASK_SUCCEEDED || out->state == UMI_TASK_FAILED ||
            out->state == UMI_TASK_CANCELLED)
            return;
        umi_thread_sleep_ms(1);
    }
    CHECK(false);
}
typedef struct RemovalWrongOwner
{
    UmiTestArchiveRemoval *removal;
    UmiTaskQueue *queue;
    UmiStatus submit, destroy;
} RemovalWrongOwner;
static int RemoveElsewhere(void *data)
{
    RemovalWrongOwner *wrong = data;
    wrong->submit = UmiTestArchiveRemovalSubmit(wrong->removal, wrong->queue);
    wrong->destroy = UmiTestArchiveRemovalDestroy(wrong->removal);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(true);
    UmiTestArchiveEntry saved = {0}, other = {0};
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &saved) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &other) == UMI_STATUS_OK);
    UmiTestArchiveRemoval *removal = NULL;
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiTestArchiveRemovalCreate(NULL, 1, &removal) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(removal == NULL);
        CHECK(UmiTestArchiveRemovalCreate(archive, 0, &removal) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveRemovalCreate(archive, 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiTestArchiveRemovalSnapshot unchanged = {0};
        unchanged.id = 91;
        CHECK(UmiTestArchiveRemovalRead(NULL, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(unchanged.id == 91);
        CHECK(UmiTestArchiveRemovalCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    CHECK(UmiTestArchiveRemovalCreate(archive, strcmp(mode, "missing") == 0 ? 99 : saved.id,
                                      &removal) == UMI_STATUS_OK);
    UmiTestArchiveRemovalSnapshot state = {0};
    CHECK(UmiTestArchiveRemovalRead(removal, &state) == UMI_STATUS_OK);
    CHECK(state.state == UMI_TASK_CREATED && !state.committed && state.status == UMI_STATUS_BUSY);
    if (strcmp(mode, "created") != 0)
    {
        UmiTaskQueue *queue = NULL;
        UmiTaskQueueConfig config = umi_task_queue_config_default();
        config.worker_count = 1;
        config.capacity = 4;
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
        if (strcmp(mode, "wrong-thread") == 0)
        {
            RemovalWrongOwner wrong = {removal, queue, UMI_STATUS_OK, UMI_STATUS_OK};
            UmiThread *thread = NULL;
            int code = 0;
            CHECK(umi_thread_start(RemoveElsewhere, &wrong, &thread) == UMI_STATUS_OK);
            CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK);
            umi_thread_destroy(thread);
            CHECK(wrong.submit == UMI_STATUS_INVALID_STATE &&
                  wrong.destroy == UMI_STATUS_INVALID_STATE);
        }
        else
        {
            RemovalGate gate;
            atomic_init(&gate.entered, 0);
            atomic_init(&gate.released, 0);
            UmiTaskConfig task_config = {0};
            task_config.function = HoldRemovalQueue;
            task_config.user_data = &gate;
            UmiTask *blocker = NULL;
            CHECK(umi_task_create(&task_config, &blocker) == UMI_STATUS_OK);
            CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
            for (unsigned i = 0; i < 5000 && !atomic_load(&gate.entered); ++i)
                umi_thread_sleep_ms(1);
            CHECK(atomic_load(&gate.entered));
            CHECK(UmiTestArchiveRemovalSubmit(removal, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiTestArchiveRemovalSubmit(removal, queue) == UMI_STATUS_OK);
            CHECK(UmiTestArchiveRemovalSubmit(removal, queue) != UMI_STATUS_OK);
            CHECK(UmiTestArchiveRemovalDestroy(removal) == UMI_STATUS_BUSY);
            if (strcmp(mode, "cancel") == 0)
                CHECK(UmiTestArchiveRemovalCancel(removal) == UMI_STATUS_OK);
            if (strcmp(mode, "busy") == 0)
                CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
            atomic_store(&gate.released, 1);
            AwaitRemoval(removal, &state);
            UmiStatus expected = strcmp(mode, "cancel") == 0    ? UMI_STATUS_CANCELLED
                                 : strcmp(mode, "busy") == 0    ? UMI_STATUS_BUSY
                                 : strcmp(mode, "missing") == 0 ? UMI_STATUS_NOT_FOUND
                                                                : UMI_STATUS_OK;
            CHECK(state.status == expected && state.committed == (expected == UMI_STATUS_OK));
            if (strcmp(mode, "busy") == 0)
            {
                CHECK(umi_data_server_in_transaction(server));
                CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
            }
            CHECK(UmiTestArchiveRead(archive, saved.id, &saved) ==
                  (state.committed ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
            CHECK(UmiTestArchiveRead(archive, other.id, &other) == UMI_STATUS_OK);
            if (strcmp(mode, "late-stop") == 0)
            {
                CHECK(UmiTestArchiveRemovalCancel(removal) == UMI_STATUS_OK);
                CHECK(UmiTestArchiveRemovalRead(removal, &state) == UMI_STATUS_OK &&
                      state.committed && state.status == UMI_STATUS_OK);
            }
            CHECK(strcmp(mode, "success") == 0 || strcmp(mode, "cancel") == 0 ||
                  strcmp(mode, "busy") == 0 || strcmp(mode, "missing") == 0 ||
                  strcmp(mode, "late-stop") == 0);
            CHECK(umi_task_wait(blocker, 5000) == UMI_STATUS_OK);
            umi_task_destroy(blocker);
        }
        umi_task_queue_destroy(queue);
    }
    CHECK(UmiTestArchiveRemovalDestroy(removal) == UMI_STATUS_OK);
done:
    CHECK(UmiTestArchiveRemovalDestroy(NULL) == UMI_STATUS_OK);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}
