/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_open.c
 * PURPOSE: Exercise worker-owned SQLite opening, late cancellation and atomic handle transfer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "fixture.h"
#include "umicom/testing/archive_open.h"
typedef struct OpenGate
{
    atomic_int entered, release;
} OpenGate;
static inline UmiStatus HoldOpenQueue(UmiTaskContext *context, void *data)
{
    (void)context;
    OpenGate *gate = data;
    atomic_store(&gate->entered, 1);
    while (!atomic_load(&gate->release))
        umi_thread_sleep_ms(1);
    return UMI_STATUS_OK;
}
static inline void AwaitOpen(UmiTestArchiveOpen *opening, UmiTestArchiveOpenSnapshot *state)
{
    for (unsigned i = 0; i < 5000U; ++i)
    {
        CHECK(UmiTestArchiveOpenRead(opening, state) == UMI_STATUS_OK);
        if (state->state == UMI_TASK_SUCCEEDED || state->state == UMI_TASK_FAILED ||
            state->state == UMI_TASK_CANCELLED)
            return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(false);
}
typedef struct ForeignOpen
{
    UmiTestArchiveOpen *opening;
    UmiTaskQueue *queue;
} ForeignOpen;
static inline int OpenElsewhere(void *data)
{
    ForeignOpen *foreign = data;
    UmiDataServer *database = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(UmiTestArchiveOpenSubmit(foreign->opening, foreign->queue) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiTestArchiveOpenTake(foreign->opening, &database, &archive) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(database == NULL && archive == NULL);
    CHECK(UmiTestArchiveOpenDestroy(foreign->opening) == UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiTestArchiveOpen *opening = NULL;
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiTestArchiveOpenCreate(NULL, "test", &opening) != UMI_STATUS_OK && opening == NULL);
        CHECK(UmiTestArchiveOpenCreate("relative.sqlite", "test", &opening) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveOpenCreate("/tmp/test.sqlite", "../scope", &opening) ==
              UMI_STATUS_INVALID_ARGUMENT);
        char oversized[66];
        memset(oversized, 'a', 65);
        oversized[65] = '\0';
        CHECK(UmiTestArchiveOpenCreate("/tmp/test.sqlite", oversized, &opening) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveOpenCreate("/tmp/test.sqlite", "", &opening) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestArchiveOpenDestroy(NULL) == UMI_STATUS_OK);
        return 0;
    }
#ifndef UMICOM_HAS_SQLITE
    return 77;
#else
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], expected[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory,
                strcmp(mode, "missing-parent") == 0 ? "absent/tests.sqlite"
                                                    : "open-caf\xc3\xa9.sqlite");
    strcpy(expected, path);
    if (strcmp(mode, "reopen") == 0 || strcmp(mode, "corrupt") == 0 || strcmp(mode, "scope") == 0)
    {
        UmiDataServer *seed = NULL;
        UmiTestArchive *archive = NULL;
        CHECK(umi_data_server_create_sqlite(path, &seed) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveCreate(seed, strcmp(mode, "scope") == 0 ? "other" : "test", &archive) ==
              UMI_STATUS_OK);
        UmiCtestJob *job = FixtureJob();
        UmiTestArchiveOrigin origin = FixtureOrigin(false);
        UmiTestArchiveEntry entry = {0};
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK);
        if (strcmp(mode, "corrupt") == 0)
            CHECK(umi_data_server_set(seed, "ctest-archive/test/next", "broken") == UMI_STATUS_OK);
        free(job);
        UmiTestArchiveDestroy(archive);
        umi_data_server_destroy(seed);
    }
    char scope[] = "test";
    CHECK(UmiTestArchiveOpenCreate(path, scope, &opening) == UMI_STATUS_OK);
    /* Copying admission inputs isolates a queued request from later UI edits. */
    path[0] = '?';
    scope[0] = 'z';
    UmiTestArchiveOpenSnapshot state = {0};
    CHECK(UmiTestArchiveOpenRead(opening, &state) == UMI_STATUS_OK);
    CHECK(strcmp(state.path, expected) == 0 && !state.ready && !state.taken);
    UmiDataServer *database = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(UmiTestArchiveOpenTake(opening, &database, &archive) == UMI_STATUS_INVALID_STATE);
    CHECK(database == NULL && archive == NULL);
    if (strcmp(mode, "created") == 0)
    {
        CHECK(UmiTestArchiveOpenDestroy(opening) == UMI_STATUS_OK);
        return 0;
    }
    UmiTaskQueueConfig config = umi_task_queue_config_default();
    config.worker_count = 1;
    config.capacity = 4;
    UmiTaskQueue *queue = NULL;
    CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    OpenGate gate;
    atomic_init(&gate.entered, 0);
    atomic_init(&gate.release, 0);
    UmiTaskConfig held = {0};
    held.function = HoldOpenQueue;
    held.user_data = &gate;
    UmiTask *blocker = NULL;
    CHECK(umi_task_create(&held, &blocker) == UMI_STATUS_OK);
    CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
    for (unsigned i = 0; i < 5000U && !atomic_load(&gate.entered); ++i)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&gate.entered));
    if (strcmp(mode, "retry") == 0)
        CHECK(UmiTestArchiveOpenSubmit(opening, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTestArchiveOpenSubmit(opening, queue) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveOpenDestroy(opening) == UMI_STATUS_BUSY);
    CHECK(UmiTestArchiveOpenTake(opening, &database, &archive) == UMI_STATUS_BUSY);
    CHECK(UmiTestArchiveOpenSubmit(opening, queue) != UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0)
        CHECK(UmiTestArchiveOpenCancel(opening) == UMI_STATUS_OK);
    atomic_store(&gate.release, 1);
    AwaitOpen(opening, &state);
    if (strcmp(mode, "wrong-thread") == 0)
    {
        ForeignOpen foreign = {opening, queue};
        UmiThread *thread = NULL;
        int code = 0;
        CHECK(umi_thread_start(OpenElsewhere, &foreign, &thread) == UMI_STATUS_OK);
        CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK && code == 0);
        umi_thread_destroy(thread);
    }
    if (strcmp(mode, "late-stop") == 0)
    {
        CHECK(state.ready && state.status == UMI_STATUS_OK);
        CHECK(UmiTestArchiveOpenCancel(opening) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveOpenRead(opening, &state) == UMI_STATUS_OK);
    }
    bool cancelled = strcmp(mode, "cancel") == 0 || strcmp(mode, "late-stop") == 0;
    if (cancelled || strcmp(mode, "corrupt") == 0 || strcmp(mode, "missing-parent") == 0)
    {
        CHECK(!state.ready && !state.taken && state.status != UMI_STATUS_OK);
        if (cancelled)
            CHECK(state.status == UMI_STATUS_CANCELLED);
        if (strcmp(mode, "corrupt") == 0)
            CHECK(state.status == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiTestArchiveOpenTake(opening, &database, &archive) == state.status);
        CHECK(database == NULL && archive == NULL);
    }
    else
    {
        CHECK(state.ready && state.status == UMI_STATUS_OK);
        if (strcmp(mode, "unclaimed") != 0)
        {
            CHECK(UmiTestArchiveOpenTake(opening, &database, &archive) == UMI_STATUS_OK);
            CHECK(database != NULL && archive != NULL);
            UmiDataServer *other_database = NULL;
            UmiTestArchive *other_archive = NULL;
            CHECK(UmiTestArchiveOpenTake(opening, &other_database, &other_archive) ==
                  UMI_STATUS_NOT_FOUND);
            CHECK(other_database == NULL && other_archive == NULL);
            if (strcmp(mode, "taken-stop") == 0)
            {
                CHECK(UmiTestArchiveOpenCancel(opening) == UMI_STATUS_OK);
                CHECK(UmiTestArchiveOpenRead(opening, &state) == UMI_STATUS_OK && state.taken);
            }
        }
    }
    CHECK(UmiTestArchiveOpenDestroy(opening) == UMI_STATUS_OK);
    /* Taken handles must outlive the opener; discarded candidates must release
     * their handles so the same local file can be opened by another owner. */
    if (strcmp(mode, "unclaimed") == 0)
    {
        CHECK(umi_data_server_create_sqlite(expected, &database) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveCreate(database, "test", &archive) == UMI_STATUS_OK);
    }
    if (archive != NULL)
    {
        UmiTestArchiveCatalog *catalog = calloc(1U, sizeof(*catalog));
        CHECK(catalog != NULL && UmiTestArchiveList(archive, catalog) == UMI_STATUS_OK);
        CHECK(catalog->count == (strcmp(mode, "reopen") == 0 ? 1U : 0U));
        free(catalog);
    }
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(database);
    umi_task_queue_destroy(queue);
    umi_task_destroy(blocker);
    return 0;
#endif
}
