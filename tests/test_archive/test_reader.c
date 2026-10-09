/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_reader.c
 * PURPOSE: Check queued history reads, cancellation, copied ownership and database refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/testing/archive_reader.h"
typedef struct ReaderGate
{
    atomic_int entered, release;
} ReaderGate;
static UmiStatus HoldWorker(UmiTaskContext *context, void *data)
{
    (void)context;
    ReaderGate *gate = data;
    atomic_store(&gate->entered, 1);
    while (!atomic_load(&gate->release))
        umi_thread_sleep_ms(1);
    return UMI_STATUS_OK;
}
static void AwaitRead(UmiTestArchiveReader *reader, UmiTestArchiveReaderSnapshot *state)
{
    for (unsigned i = 0; i < 5000; ++i)
    {
        CHECK(UmiTestArchiveReaderRead(reader, state) == UMI_STATUS_OK);
        if (state->state == UMI_TASK_SUCCEEDED || state->state == UMI_TASK_FAILED ||
            state->state == UMI_TASK_CANCELLED)
            return;
        umi_thread_sleep_ms(1);
    }
    CHECK(false);
}
typedef struct WrongOwner
{
    UmiTestArchiveReader *reader;
    UmiStatus status;
} WrongOwner;
static int DestroyElsewhere(void *data)
{
    WrongOwner *wrong = data;
    wrong->status = UmiTestArchiveReaderDestroy(wrong->reader);
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
    UmiTestArchiveEntry saved = {0};
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &saved) == UMI_STATUS_OK);
    UmiTestArchiveReader *reader = NULL;
    UmiTestArchiveReadKind kind = strcmp(mode, "catalog") == 0 ? UMI_TEST_ARCHIVE_READ_CATALOG
                                                               : UMI_TEST_ARCHIVE_READ_ATTEMPT;
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiTestArchiveReaderCreate(archive, (UmiTestArchiveReadKind)-1, 1, 0, &reader) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              reader == NULL);
        CHECK(UmiTestArchiveReaderCreate(archive, UMI_TEST_ARCHIVE_READ_CATALOG, 1, 0, &reader) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveReaderCreate(archive, UMI_TEST_ARCHIVE_READ_ATTEMPT, 0, 0, &reader) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveReaderCreate(archive, UMI_TEST_ARCHIVE_READ_ATTEMPT, 1,
                                         UMI_CTEST_JOB_MAX_ATTEMPTS,
                                         &reader) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    CHECK(UmiTestArchiveReaderCreate(archive, kind,
                                     kind == UMI_TEST_ARCHIVE_READ_CATALOG ? 0
                                     : strcmp(mode, "missing") == 0        ? 99
                                                                           : saved.id,
                                     kind == UMI_TEST_ARCHIVE_READ_CATALOG ? 0
                                     : strcmp(mode, "index") == 0          ? 4
                                                                           : 2,
                                     &reader) == UMI_STATUS_OK);
    UmiTestArchiveAttempt *copy = calloc(1, sizeof(*copy));
    UmiTestArchiveCatalog *catalog = calloc(1, sizeof(*catalog));
    CHECK(copy != NULL && catalog != NULL);
    copy->entry.id = 999;
    CHECK(
        UmiTestArchiveReaderAttempt(reader, copy) ==
        (kind == UMI_TEST_ARCHIVE_READ_CATALOG ? UMI_STATUS_INVALID_STATE : UMI_STATUS_NOT_FOUND));
    CHECK(copy->entry.id == 999);
    if (strcmp(mode, "wrong-thread") == 0)
    {
        WrongOwner wrong = {reader, UMI_STATUS_OK};
        UmiThread *thread = NULL;
        int code = 0;
        CHECK(umi_thread_start(DestroyElsewhere, &wrong, &thread) == UMI_STATUS_OK);
        CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK);
        umi_thread_destroy(thread);
        CHECK(wrong.status == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(mode, "created") != 0)
    {
        UmiTaskQueue *queue = NULL;
        UmiTaskQueueConfig config = umi_task_queue_config_default();
        config.worker_count = 1;
        config.capacity = 4;
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
        ReaderGate gate;
        atomic_init(&gate.entered, 0);
        atomic_init(&gate.release, 0);
        UmiTaskConfig gate_config = {0};
        gate_config.function = HoldWorker;
        gate_config.user_data = &gate;
        UmiTask *blocker = NULL;
        CHECK(umi_task_create(&gate_config, &blocker) == UMI_STATUS_OK);
        CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
        for (unsigned i = 0; i < 5000 && !atomic_load(&gate.entered); ++i)
            umi_thread_sleep_ms(1);
        CHECK(atomic_load(&gate.entered));
        CHECK(UmiTestArchiveReaderSubmit(reader, queue) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveReaderDestroy(reader) == UMI_STATUS_BUSY);
        CHECK(UmiTestArchiveReaderSubmit(reader, queue) != UMI_STATUS_OK);
        if (strcmp(mode, "cancel") == 0)
            CHECK(UmiTestArchiveReaderCancel(reader) == UMI_STATUS_OK);
        if (strcmp(mode, "busy") == 0)
            CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        atomic_store(&gate.release, 1);
        UmiTestArchiveReaderSnapshot state = {0};
        AwaitRead(reader, &state);
        if (strcmp(mode, "cancel") == 0 || strcmp(mode, "missing") == 0 ||
            strcmp(mode, "index") == 0 || strcmp(mode, "busy") == 0)
        {
            UmiStatus expected = strcmp(mode, "cancel") == 0 ? UMI_STATUS_CANCELLED
                                 : strcmp(mode, "busy") == 0 ? UMI_STATUS_BUSY
                                                             : UMI_STATUS_NOT_FOUND;
            CHECK(!state.ready && state.status == expected);
            CHECK(UmiTestArchiveReaderAttempt(reader, copy) == UMI_STATUS_NOT_FOUND);
            CHECK(copy->entry.id == 999);
            if (strcmp(mode, "busy") == 0)
            {
                CHECK(umi_data_server_in_transaction(server));
                CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
            }
        }
        else
        {
            CHECK(strcmp(mode, "catalog") == 0 || strcmp(mode, "attempt") == 0 ||
                  strcmp(mode, "detached") == 0 || strcmp(mode, "late-stop") == 0);
            CHECK(state.ready && state.status == UMI_STATUS_OK);
            if (strcmp(mode, "late-stop") == 0)
            {
                CHECK(UmiTestArchiveReaderCancel(reader) == UMI_STATUS_OK);
                CHECK(UmiTestArchiveReaderRead(reader, &state) == UMI_STATUS_OK && state.ready);
            }
            if (strcmp(mode, "detached") == 0)
                CHECK(UmiTestArchiveRemove(archive, saved.id) == UMI_STATUS_OK);
            if (kind == UMI_TEST_ARCHIVE_READ_CATALOG)
            {
                CHECK(UmiTestArchiveReaderCatalog(reader, catalog) == UMI_STATUS_OK);
                CHECK(catalog->count == 1 && catalog->entries[0].id == saved.id);
            }
            else
            {
                CHECK(UmiTestArchiveReaderAttempt(reader, copy) == UMI_STATUS_OK);
                CHECK(copy->entry.id == saved.id && copy->attempt == 2);
                CHECK(copy->result.state == UMI_TEST_STATE_FAILED);
                CHECK(strcmp(copy->request.test_id, copy->result.test_id) == 0);
                CHECK(strcmp(copy->result.output, job->results[2].output) == 0);
                CHECK(UmiTestArchiveReaderCatalog(reader, catalog) == UMI_STATUS_INVALID_STATE);
            }
        }
        CHECK(UmiTestArchiveReaderDestroy(reader) == UMI_STATUS_OK);
        reader = NULL;
        umi_task_queue_destroy(queue);
        umi_task_destroy(blocker);
    }
    CHECK(UmiTestArchiveReaderDestroy(reader) == UMI_STATUS_OK);
    free(copy);
    free(catalog);
done:
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}
