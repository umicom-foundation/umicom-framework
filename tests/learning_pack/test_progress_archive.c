/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_progress_archive.c
 *
 * PURPOSE:
 *   Verify portable checkpoint archives preserve history and refuse malformed imports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "progress_reader_fixture.h"
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    int result = 0;
    UmiDataServer *source = NULL, *destination = NULL;
    UmiLearningProgress *original = NULL, *target = NULL, *second = NULL;
    UmiLearningReader *reader = NULL, *changed = NULL;
    UmiLearningProgressEvent event, guard;
    UmiLearningProgressSnapshot snapshot;
    char *archive = NULL, *modified = NULL;
    size_t size = 0U;
    CHECK(umi_data_server_create_memory(&source) == UMI_STATUS_OK);
    CHECK(umi_data_server_create_memory(&destination) == UMI_STATUS_OK);
    CHECK(Reader(argv[2], 1U, "Archive lesson", "guide", "archive.fixture", "guide", &reader) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(source, "learner", &original) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(destination, strcmp(name, "wrong-learner") == 0 ? "other" : "learner", &target) == UMI_STATUS_OK);
    if (strcmp(name, "empty") != 0)
        CHECK(UmiLearningProgressRecord(original, reader, "read", "e1", &event) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressExport(original, NULL, 0U, &size) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(size > 0U && size <= UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES);
    archive = malloc(size + 1U);
    modified = malloc(size * 2U + 64U);
    CHECK(archive != NULL && modified != NULL);
    CHECK(UmiLearningProgressExport(original, archive, size + 1U, &size) == UMI_STATUS_OK);
    CHECK(archive[size] == '\0' && archive[size - 1U] == '\n' && strlen(archive) == size);
    memset(&event, 0xa5, sizeof(event)); memcpy(&guard, &event, sizeof(guard));
    if (strcmp(name, "short-buffer") == 0) {
        char small[8], saved[8]; memset(small, 'x', sizeof(small)); memcpy(saved, small, sizeof(saved));
        size_t required = 0U;
        CHECK(UmiLearningProgressExport(original, small, sizeof(small), &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(required == size && memcmp(small, saved, sizeof(small)) == 0);
        required = 321U;
        CHECK(UmiLearningProgressExport(original, NULL, 1U, &required) == UMI_STATUS_INVALID_ARGUMENT && required == 321U);
        goto cleanup;
    }
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningProgressExport(original, archive, size + 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressExport(NULL, archive, size + 1U, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressImportEmpty(NULL, archive, size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressImportEmpty(target, NULL, size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressImportEmpty(target, archive, 0U) == UMI_STATUS_PARSE_ERROR);
        goto cleanup;
    }
    if (strcmp(name, "over-limit") == 0) {
        CHECK(UmiLearningProgressImportEmpty(target, archive, UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        goto unchanged;
    }
    if (strcmp(name, "nonempty") == 0 || strcmp(name, "stale-nonempty") == 0) {
        UmiLearningProgress *writer = target;
        if (strcmp(name, "stale-nonempty") == 0) {
            CHECK(UmiLearningProgressOpen(destination, "learner", &second) == UMI_STATUS_OK);
            writer = second;
        }
        CHECK(UmiLearningProgressRecord(writer, reader, "review", "local-evidence", &event) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiLearningProgressFindCurrent(writer, reader, "review", &event) == UMI_STATUS_OK);
        CHECK(strcmp(event.evidence_reference, "local-evidence") == 0);
        CHECK(umi_data_server_count(destination) == 2U);
        goto cleanup;
    }
    if (strcmp(name, "transaction-busy") == 0) {
        CHECK(umi_data_server_begin(destination) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(destination));
        CHECK(umi_data_server_rollback(destination) == UMI_STATUS_OK);
        goto unchanged;
    }
    if (strcmp(name, "wrong-learner") == 0) {
        CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_INVALID_ARGUMENT);
        goto unchanged;
    }
    if (strcmp(name, "crlf") == 0) {
        size_t at = 0U;
        for (size_t i = 0U; i < size; ++i) {
            if (archive[i] == '\n') modified[at++] = '\r';
            modified[at++] = archive[i];
        }
        modified[at] = '\0';
        CHECK(UmiLearningProgressImportEmpty(target, modified, at) == UMI_STATUS_OK);
    } else if (strcmp(name, "roundtrip") == 0 || strcmp(name, "empty") == 0 ||
               strcmp(name, "changed-revision") == 0 || strcmp(name, "source-independent") == 0) {
        if (strcmp(name, "source-independent") == 0) {
            UmiLearningProgressClose(original); original = NULL;
            umi_data_server_destroy(source); source = NULL;
        }
        CHECK(UmiLearningProgressImportEmpty(target, archive, size) == UMI_STATUS_OK);
    } else {
        memcpy(modified, archive, size + 1U);
        size_t length = size;
        char *newline = strchr(archive, '\n');
        CHECK(newline != NULL);
        if (strcmp(name, "version") == 0) modified[3] = '2';
        else if (strcmp(name, "head-count") == 0) modified[(size_t)(newline - archive) - 1U] = '2';
        else if (strcmp(name, "leading-zero") == 0) {
            size_t at = (size_t)(newline - archive) - 1U;
            memmove(modified + at + 1U, modified + at, size - at + 1U);
            modified[at] = '0'; ++length;
        } else if (strcmp(name, "missing-newline") == 0) --length;
        else if (strcmp(name, "trailing-data") == 0) { modified[length++] = 'x'; modified[length] = '\0'; }
        else if (strcmp(name, "embedded-nul") == 0) modified[length / 2U] = '\0';
        else if (strcmp(name, "malformed-event") == 0) modified[(size_t)(newline - archive) + 1U] = 'X';
        else if (strcmp(name, "duplicate-event") == 0) {
            modified[(size_t)(newline - archive) - 1U] = '2';
            size_t event_length = strlen(newline + 1);
            memcpy(modified + length, newline + 1, event_length);
            length += event_length; modified[length] = '\0';
        } else { result = 2; goto cleanup; }
        CHECK(UmiLearningProgressImportEmpty(target, modified, length) == UMI_STATUS_PARSE_ERROR);
        goto unchanged;
    }
    CHECK(UmiLearningProgressCopySnapshot(target, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.event_count == (strcmp(name, "empty") == 0 ? 0U : 1U));
    if (strcmp(name, "empty") == 0) {
        CHECK(umi_data_server_count(destination) == 0U);
        goto cleanup;
    }
    CHECK(UmiLearningProgressFindCurrent(target, reader, "read", &event) == UMI_STATUS_OK);
    CHECK(event.sequence == 1U && strcmp(event.evidence_reference, "e1") == 0);
    if (strcmp(name, "changed-revision") == 0) {
        CHECK(Reader(argv[2], 2U, "Archive lesson", "guide", "archive.fixture", "guide", &changed) == UMI_STATUS_OK);
        memcpy(&guard, &event, sizeof(guard));
        CHECK(UmiLearningProgressFindCurrent(target, changed, "read", &event) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
    }
    CHECK(UmiLearningProgressOpen(destination, "learner", &second) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressFindCurrent(second, reader, "read", &event) == UMI_STATUS_OK);
    goto cleanup;
unchanged:
    CHECK(UmiLearningProgressCopySnapshot(target, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 0U);
    CHECK(umi_data_server_count(destination) == 0U && !umi_data_server_in_transaction(destination));
    CHECK(UmiLearningProgressFindCurrent(target, reader, "read", &event) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
cleanup:
    if (destination != NULL && umi_data_server_in_transaction(destination)) (void)umi_data_server_rollback(destination);
    UmiLearningProgressClose(second);
    UmiLearningProgressClose(target);
    UmiLearningProgressClose(original);
    UmiLearningReaderDestroy(changed);
    UmiLearningReaderDestroy(reader);
    umi_data_server_destroy(destination);
    umi_data_server_destroy(source);
    free(modified); free(archive);
    return result;
}
