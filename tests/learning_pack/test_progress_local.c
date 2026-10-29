/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_progress_local.c
 *
 * PURPOSE:
 *   Check explicit private checkpoint requests, refusal before I/O and persistent receipts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/learning_progress_local.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/secure_random.h"
#include "progress_reader_fixture.h"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
static UmiStatus DatabasePath(const char *root, char out[UMI_PATH_CAPACITY])
{
    unsigned char random[16];
    UmiStatus status = UmiSecureRandomBytes(random, sizeof(random));
    if (status != UMI_STATUS_OK) return status;
    char token[33];
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0U; i < sizeof(random); ++i) {
        token[i * 2U] = digits[random[i] >> 4U];
        token[i * 2U + 1U] = digits[random[i] & 15U];
    }
    token[32] = '\0';
    int size = snprintf(out, UMI_PATH_CAPACITY, "%s/local-%s.sqlite3", root, token);
    return size < 0 || (size_t)size >= UMI_PATH_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningReader *reader = NULL, *changed = NULL;
    UmiDataServer *server = NULL;
    UmiLearningProgressLocalRequest request = {0}, invalid;
    UmiLearningProgressLocalReceipt receipt, guard;
    UmiLearningReaderSnapshot before, after;
    request.action = UMI_LEARNING_PROGRESS_RECORD;
    CHECK(DatabasePath(root, request.database_path) == UMI_STATUS_OK);
    memcpy(request.learner_id, "learner", 8U);
    memcpy(request.checkpoint, "reviewed", 9U);
    memcpy(request.evidence_reference, "practice-1", 11U);
    CHECK(Reader(root, 1U, "First lesson", strcmp(name, "wrong-kind") == 0 ? "example" : "lesson",
        "local.fixture", "lesson", &reader) == UMI_STATUS_OK);
    CHECK(UmiLearningReaderCopySnapshot(reader, &before) == UMI_STATUS_OK);
    memset(&receipt, 0xa5, sizeof(receipt)); memcpy(&guard, &receipt, sizeof(guard));
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningProgressLocalValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressLocalExecute(NULL, &request, &receipt) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressLocalExecute(reader, NULL, &receipt) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressLocalExecute(reader, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto no_storage;
    }
    if (strcmp(name, "wrong-kind") == 0) {
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_INVALID_STATE);
        goto no_storage;
    }
    invalid = request;
    int admission = 1;
    if (strcmp(name, "action") == 0) invalid.action = (UmiLearningProgressLocalAction)99;
    else if (strcmp(name, "relative") == 0) memcpy(invalid.database_path, "relative.sqlite3", 17U);
    else if (strcmp(name, "path-control") == 0) invalid.database_path[2] = '\n';
    else if (strcmp(name, "unterminated-path") == 0) memset(invalid.database_path, 'x', sizeof(invalid.database_path));
    else if (strcmp(name, "learner") == 0) memcpy(invalid.learner_id, "private/name", 13U);
    else if (strcmp(name, "checkpoint") == 0) invalid.checkpoint[0] = '\0';
    else if (strcmp(name, "evidence") == 0) memcpy(invalid.evidence_reference, "personal note", 14U);
    else if (strcmp(name, "unterminated-id") == 0) memset(invalid.learner_id, 'x', sizeof(invalid.learner_id));
    else if (strcmp(name, "inspect-evidence") == 0) invalid.action = UMI_LEARNING_PROGRESS_INSPECT;
    else admission = 0;
    if (admission) {
        CHECK(UmiLearningProgressLocalValidate(&invalid) != UMI_STATUS_OK);
        CHECK(UmiLearningProgressLocalExecute(reader, &invalid, &receipt) != UMI_STATUS_OK);
        goto no_storage;
    }
    if (strcmp(name, "read-missing") == 0) {
        request.action = UMI_LEARNING_PROGRESS_INSPECT; request.evidence_reference[0] = '\0';
        UmiStatus status = UmiLearningProgressLocalExecute(reader, &request, &receipt);
        if (status == UMI_STATUS_UNAVAILABLE) { fprintf(stderr, "SKIP: SQLite Data Server backend unavailable.\n"); result = 77; goto cleanup; }
        CHECK(status != UMI_STATUS_OK && receipt.status == status && !receipt.storage_opened);
        CHECK(!umi_fs_exists(request.database_path));
        goto cleanup;
    }
    if (strcmp(name, "empty-journal") == 0 || strcmp(name, "corrupt-journal") == 0) {
        UmiStatus status = umi_data_server_create_sqlite(request.database_path, &server);
        if (status == UMI_STATUS_UNAVAILABLE) { fprintf(stderr, "SKIP: SQLite Data Server backend unavailable.\n"); result = 77; goto cleanup; }
        CHECK(status == UMI_STATUS_OK);
        if (strcmp(name, "corrupt-journal") == 0)
            CHECK(umi_data_server_set(server, "learning.progress/learner/head", "unknown-format") == UMI_STATUS_OK);
        umi_data_server_destroy(server); server = NULL;
        request.action = UMI_LEARNING_PROGRESS_INSPECT; request.evidence_reference[0] = '\0';
        status = UmiLearningProgressLocalExecute(reader, &request, &receipt);
        if (strcmp(name, "empty-journal") == 0)
            CHECK(status == UMI_STATUS_NOT_FOUND && receipt.journal_loaded && receipt.journal.event_count == 0U);
        else CHECK(status == UMI_STATUS_PARSE_ERROR && receipt.storage_opened && !receipt.journal_loaded);
        CHECK(!receipt.has_event && !receipt.write_attempted);
        goto cleanup;
    }
    UmiStatus status = UmiLearningProgressLocalExecute(reader, &request, &receipt);
    if (status == UMI_STATUS_UNAVAILABLE) { fprintf(stderr, "SKIP: SQLite Data Server backend unavailable.\n"); result = 77; goto cleanup; }
    CHECK(status == UMI_STATUS_OK && receipt.status == status);
    CHECK(receipt.storage_opened && receipt.journal_loaded && receipt.write_attempted && receipt.has_event);
    CHECK(receipt.event.sequence == 1U && receipt.journal.event_count == 1U && !receipt.journal.recovery_required);
    CHECK(receipt.selection.pack.revision == 1U && strcmp(receipt.request.database_path, request.database_path) == 0);
    CHECK(receipt.journal.backend == UMI_DATA_BACKEND_SQLITE);
    if (strcmp(name, "duplicate") == 0) {
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(receipt.write_attempted && !receipt.has_event && receipt.journal.event_count == 1U);
    } else if (strcmp(name, "new-evidence") == 0) {
        memcpy(request.evidence_reference, "practice-2", 11U);
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.event.sequence == 2U && receipt.journal.event_count == 2U);
    } else if (strcmp(name, "reader-snapshot") == 0) {
        CHECK(UmiRootedFileWrite(root, "lesson.html", "changed", 7U) == UMI_STATUS_OK);
        /* Recording the displayed verified snapshot does not re-open its file. */
        memcpy(request.checkpoint, "discussed", 10U);
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_OK);
        CHECK(strcmp(receipt.event.sha256, before.selection.resource.sha256) == 0);
    } else CHECK(strcmp(name, "record-reopen") == 0 || strcmp(name, "revision") == 0 ||
        strcmp(name, "learner-separation") == 0);
    request.action = UMI_LEARNING_PROGRESS_INSPECT; request.evidence_reference[0] = '\0';
    if (strcmp(name, "learner-separation") == 0) {
        memcpy(request.learner_id, "other", 6U);
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_NOT_FOUND);
        CHECK(receipt.journal_loaded && receipt.journal.event_count == 0U && !receipt.has_event);
    } else if (strcmp(name, "revision") == 0) {
        CHECK(Reader(root, 2U, "Second lesson", "lesson", "local.fixture", "lesson", &changed) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressLocalExecute(changed, &request, &receipt) == UMI_STATUS_NOT_FOUND);
        CHECK(receipt.journal_loaded && receipt.journal.event_count == 1U && !receipt.has_event);
        CHECK(receipt.selection.pack.revision == 2U);
    } else {
        CHECK(UmiLearningProgressLocalExecute(reader, &request, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.has_event && !receipt.write_attempted);
        if (strcmp(name, "new-evidence") == 0)
            CHECK(receipt.event.sequence == 2U && strcmp(receipt.event.evidence_reference, "practice-2") == 0);
    }
    CHECK(UmiLearningReaderCopySnapshot(reader, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && after.history_count == before.history_count);
    goto cleanup;
no_storage:
    CHECK(memcmp(&receipt, &guard, sizeof(receipt)) == 0);
    CHECK(!umi_fs_exists(request.database_path));
cleanup:
    umi_data_server_destroy(server);
    UmiLearningReaderDestroy(changed); UmiLearningReaderDestroy(reader);
    return result;
}
