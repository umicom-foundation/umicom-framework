/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_progress_faults.c
 *
 * PURPOSE:
 *   Check that uncertain writes retire progress owners without publishing false success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_progress.h"
#include "progress_reader_fixture.h"
#include <stdio.h>
#include <string.h>

static int fail_set_call, set_calls, fail_commit, commit_then_fail, fail_rollback;
static UmiStatus ProgressTestSet(UmiDataServer *server, const char *key, const char *value)
{
    ++set_calls;
    if (fail_set_call != 0 && set_calls == fail_set_call) return UMI_STATUS_IO_ERROR;
    return umi_data_server_set(server, key, value);
}
static UmiStatus ProgressTestCommit(UmiDataServer *server)
{
    if (fail_commit && !commit_then_fail) return UMI_STATUS_IO_ERROR;
    UmiStatus status = umi_data_server_commit(server);
    return status == UMI_STATUS_OK && fail_commit ? UMI_STATUS_IO_ERROR : status;
}
static UmiStatus ProgressTestRollback(UmiDataServer *server)
{
    if (fail_rollback) return UMI_STATUS_IO_ERROR;
    return umi_data_server_rollback(server);
}
/* Compile the real progress operations with three narrow storage seams. The
 * normal Data Server still owns transactions and records. Faults are selected
 * only in this test translation unit; no production backdoor is introduced. */
#define umi_data_server_set ProgressTestSet
#define umi_data_server_commit ProgressTestCommit
#define umi_data_server_rollback ProgressTestRollback
#include "../../src/teacher/learning_progress_storage.c"
#include "../../src/teacher/learning_progress.c"
#include "../../src/teacher/learning_progress_archive.c"
#undef umi_data_server_set
#undef umi_data_server_commit
#undef umi_data_server_rollback

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
static int ImportFault(const char *name, const char *root)
{
    int result = 0;
    UmiDataServer *source = NULL, *destination = NULL;
    UmiLearningProgress *original = NULL, *target = NULL, *reopened = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningProgressEvent event;
    UmiLearningProgressSnapshot snapshot;
    char *archive = NULL;
    size_t size = 0U;
    CHECK(umi_data_server_create_memory(&source) == UMI_STATUS_OK);
    CHECK(umi_data_server_create_memory(&destination) == UMI_STATUS_OK);
    CHECK(Reader(root, 1U, "Import fault lesson", "lesson", "import.fixture", "lesson", &reader) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(source, "learner", &original) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressRecord(original, reader, "read", "e1", &event) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressExport(original, NULL, 0U, &size) == UMI_STATUS_CAPACITY_EXCEEDED);
    archive = malloc(size + 1U);
    CHECK(archive != NULL);
    CHECK(UmiLearningProgressExport(original, archive, size + 1U, &size) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(destination, "learner", &target) == UMI_STATUS_OK);
    set_calls = 0;
    if (strcmp(name, "import-event-write") == 0) fail_set_call = 1;
    else if (strcmp(name, "import-head-write") == 0) fail_set_call = 2;
    else if (strcmp(name, "import-commit-before") == 0) fail_commit = 1;
    else if (strcmp(name, "import-commit-after") == 0) { fail_commit = 1; commit_then_fail = 1; }
    else if (strcmp(name, "import-rollback") == 0) { fail_set_call = 2; fail_rollback = 1; }
    else { result = 2; goto cleanup; }
    CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_IO_ERROR);
    CHECK(UmiLearningProgressCopySnapshot(target, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 0U);
    CHECK(snapshot.recovery_required == (fail_commit || fail_rollback));
    if (snapshot.recovery_required) {
        size_t unchanged = 111U;
        CHECK(UmiLearningProgressExport(target, NULL, 0U, &unchanged) == UMI_STATUS_INVALID_STATE && unchanged == 111U);
        CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningProgressRecord(target, reader, "review", "e2", &event) == UMI_STATUS_INVALID_STATE);
    } else {
        CHECK(!umi_data_server_in_transaction(destination) && umi_data_server_count(destination) == 0U);
    }
    fail_set_call = 0; fail_commit = 0; fail_rollback = 0;
    if (umi_data_server_in_transaction(destination)) CHECK(umi_data_server_rollback(destination) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(destination, "learner", &reopened) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressCopySnapshot(reopened, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.event_count == (commit_then_fail ? 1U : 0U));
cleanup:
    fail_rollback = 0;
    if (destination != NULL && umi_data_server_in_transaction(destination)) (void)umi_data_server_rollback(destination);
    UmiLearningProgressClose(reopened);
    UmiLearningProgressClose(target);
    UmiLearningProgressClose(original);
    UmiLearningReaderDestroy(reader);
    umi_data_server_destroy(destination);
    umi_data_server_destroy(source);
    free(archive);
    return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    if (strncmp(name, "import-", 7U) == 0) return ImportFault(name, argv[2]);
    int result = 0;
    UmiDataServer *server = NULL;
    UmiLearningProgress *owner = NULL, *reopened = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningProgressEvent event, guard;
    UmiLearningProgressSnapshot snapshot;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(Reader(argv[2], 1U, "Known lesson", "lesson", "fault.fixture", "lesson", &reader) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(server, "learner", &owner) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressRecord(owner, reader, "read", "e1", &event) == UMI_STATUS_OK);
    memcpy(&guard, &event, sizeof(guard));
    set_calls = 0;
    if (strcmp(name, "event-write") == 0) fail_set_call = 1;
    else if (strcmp(name, "head-write") == 0) fail_set_call = 2;
    else if (strcmp(name, "commit-before") == 0) fail_commit = 1;
    else if (strcmp(name, "commit-after") == 0) { fail_commit = 1; commit_then_fail = 1; }
    else if (strcmp(name, "rollback-write") == 0) { fail_set_call = 2; fail_rollback = 1; }
    else if (strcmp(name, "rollback-refresh") == 0) fail_rollback = 1;
    else { result = 2; goto cleanup; }
    if (strcmp(name, "rollback-refresh") == 0)
        CHECK(UmiLearningProgressRefresh(owner) == UMI_STATUS_IO_ERROR);
    else
        CHECK(UmiLearningProgressRecord(owner, reader, "review", "e2", &event) == UMI_STATUS_IO_ERROR);
    CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
    CHECK(UmiLearningProgressCopySnapshot(owner, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 1U);
    int uncertain = fail_commit || fail_rollback;
    CHECK(snapshot.recovery_required == uncertain);
    if (uncertain) {
        CHECK(UmiLearningProgressRecord(owner, reader, "review", "e2", &event) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningProgressRefresh(owner) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &event) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressEventAt(owner, 0U, &event) == UMI_STATUS_OK && event.sequence == 1U);
    } else {
        CHECK(!umi_data_server_in_transaction(server));
        CHECK(umi_data_server_count(server) == 2U);
    }
    /* Repair only the deliberately injected test seam, then inspect the real
     * storage. A failed commit result may still have made the event durable. */
    fail_set_call = 0; fail_commit = 0; fail_rollback = 0;
    if (umi_data_server_in_transaction(server)) CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(server, "learner", &reopened) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressCopySnapshot(reopened, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.event_count == (commit_then_fail ? 2U : 1U));
    if (commit_then_fail) {
        CHECK(UmiLearningProgressFindCurrent(reopened, reader, "review", &event) == UMI_STATUS_OK);
        CHECK(strcmp(event.evidence_reference, "e2") == 0);
    } else {
        CHECK(UmiLearningProgressFindCurrent(reopened, reader, "review", &event) == UMI_STATUS_NOT_FOUND);
    }
cleanup:
    fail_rollback = 0;
    if (server != NULL && umi_data_server_in_transaction(server)) (void)umi_data_server_rollback(server);
    UmiLearningProgressClose(reopened);
    UmiLearningProgressClose(owner);
    UmiLearningReaderDestroy(reader);
    umi_data_server_destroy(server);
    return result;
}
