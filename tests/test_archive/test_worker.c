/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_worker.c
 * PURPOSE: Exercise nonblocking archive cancellation, borrowed lifetime and published commit status.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static void WaitTerminal(UmiTestArchiveWrite *writer, UmiTestArchiveWriteSnapshot *out)
{
    for (unsigned i = 0; i < 5000U; ++i)
    {
        CHECK(UmiTestArchiveWriteRead(writer, out) == UMI_STATUS_OK);
        if (out->state == UMI_TASK_SUCCEEDED || out->state == UMI_TASK_FAILED ||
            out->state == UMI_TASK_CANCELLED)
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
    UmiTestArchiveWrite *writer = NULL;
    if (strcmp(name, "active") == 0)
    {
        job->snapshot.state = UMI_TASK_RUNNING;
        CHECK(UmiTestArchiveWriteCreate(archive, job, &origin, &writer) == UMI_STATUS_BUSY && writer == NULL);
    }
    else
    {
        CHECK(UmiTestArchiveWriteCreate(archive, job, &origin, &writer) == UMI_STATUS_OK);
        if (strcmp(name, "created") == 0)
            CHECK(UmiTestArchiveWriteDestroy(writer) == UMI_STATUS_OK);
        else
        {
            UmiTaskQueue *queue = NULL;
            UmiTaskQueueConfig config = umi_task_queue_config_default();
            config.worker_count = 1;
            config.capacity = 4;
            CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
            bool block = strcmp(name, "cancel") == 0 || strcmp(name, "lifetime") == 0;
            atomic_store(&job->block, block);
            if (strcmp(name, "storage-failure") == 0)
                CHECK(umi_data_server_set(server, "ctest-archive/test/next", "invalid") == UMI_STATUS_OK);
            CHECK(UmiTestArchiveWriteSubmit(writer, queue) == UMI_STATUS_OK);
            if (block)
            {
                for (unsigned i = 0; i < 5000U && !atomic_load(&job->entered); ++i)
                    umi_thread_sleep_ms(1);
                CHECK(atomic_load(&job->entered));
                CHECK(UmiTestArchiveWriteDestroy(writer) == UMI_STATUS_BUSY);
                if (strcmp(name, "cancel") == 0)
                    CHECK(UmiTestArchiveWriteCancel(writer) == UMI_STATUS_OK);
                atomic_store(&job->block, 0);
            }
            UmiTestArchiveWriteSnapshot state = {0};
            WaitTerminal(writer, &state);
            if (strcmp(name, "cancel") == 0)
            {
                CHECK(state.saved.id == 0 && state.status == UMI_STATUS_CANCELLED);
                FixtureEmpty(archive);
            }
            else if (strcmp(name, "storage-failure") == 0)
                CHECK(state.saved.id == 0 && state.status == UMI_STATUS_PARSE_ERROR);
            else
            {
                CHECK(strcmp(name, "success") == 0 || strcmp(name, "lifetime") == 0 ||
                      strcmp(name, "late-stop") == 0);
                CHECK(state.saved.id == 1 && state.status == UMI_STATUS_OK);
                if (strcmp(name, "late-stop") == 0)
                {
                    CHECK(UmiTestArchiveWriteCancel(writer) == UMI_STATUS_OK);
                    CHECK(UmiTestArchiveWriteRead(writer, &state) == UMI_STATUS_OK && state.saved.id == 1);
                }
            }
            CHECK(UmiTestArchiveWriteDestroy(writer) == UMI_STATUS_OK);
            umi_task_queue_destroy(queue);
        }
    }
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}
