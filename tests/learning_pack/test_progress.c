/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_progress.c
 *
 * PURPOSE:
 *   Exercise revision-bound checkpoints, journal conflicts and persistent reopening.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_progress.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/secure_random.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

/* Storage and transaction-fault tests share the same real verified reader.
 * Retain the original local fixture for comparison. */
#if 0
static UmiStatus Reader(const char *root, uint64_t revision, const char *body,
    const char *kind, const char *pack_id, const char *resource_id, UmiLearningReader **out)
{
    char hash[UMI_SHA256_HEX_CAPACITY], json[1600];
    *out = NULL;
    UmiStatus status = UmiSha256Buffer(body, strlen(body), hash);
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(root, "lesson.html", body, strlen(body));
    if (status != UMI_STATUS_OK) return status;
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Progress fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":%" PRIu64 ","
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"%s\",\"title\":\"Lesson\",\"topic\":\"records\",\"kind\":\"%s\","
        "\"path\":\"lesson.html\",\"sha256\":\"%s\"}]}", pack_id, revision, resource_id, kind, hash);
    if (n < 0 || (size_t)n >= sizeof(json)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningPack *pack = NULL;
    status = UmiLearningPackCreate(json, (size_t)n, root, (UmiVersion){0U, 9U, 0U}, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreate(&pack, 4096U, out);
    UmiLearningPackDestroy(pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderOpen(*out, resource_id);
    if (status != UMI_STATUS_OK) { UmiLearningReaderDestroy(*out); *out = NULL; }
    return status;
}
#endif
#include "progress_reader_fixture.h"
static UmiStatus DatabaseName(const char *root, char out[2048])
{
    unsigned char random[16];
    UmiStatus status = UmiSecureRandomBytes(random, sizeof(random));
    if (status != UMI_STATUS_OK) return status;
    char id[33];
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0U; i < sizeof(random); ++i) {
        id[i * 2U] = hex[random[i] >> 4U];
        id[i * 2U + 1U] = hex[random[i] & 15U];
    }
    id[32] = '\0';
    int n = snprintf(out, 2048U, "%s/progress-%s.sqlite3", root, id);
    return n < 0 || (size_t)n >= 2048U ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiDataServer *server = NULL;
    UmiLearningProgress *owner = NULL, *other = NULL;
    UmiLearningReader *reader = NULL, *changed = NULL;
    UmiLearningProgressEvent event, guard, found;
    UmiLearningProgressSnapshot snapshot;
    char database[2048] = {0}, wire[640];
    int sqlite_case = strcmp(name, "sqlite") == 0 || strcmp(name, "readonly") == 0;
    if (sqlite_case) {
        CHECK(DatabaseName(root, database) == UMI_STATUS_OK);
        UmiStatus opened = umi_data_server_create_sqlite(database, &server);
        if (opened == UMI_STATUS_UNAVAILABLE) {
            fprintf(stderr, "SQLite backend unavailable; persistent progress coverage skipped.\n");
            result = 77; goto cleanup;
        }
        CHECK(opened == UMI_STATUS_OK);
    } else {
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    }
    CHECK(Reader(root, 1U, "Original lesson", "lesson", "progress.fixture", "lesson", &reader) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressOpen(server, "learner", &owner) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressCopySnapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.event_count == 0U && !snapshot.recovery_required);
    memset(&event, 0xa5, sizeof(event));
    memcpy(&guard, &event, sizeof(guard));
    if (strcmp(name, "empty") == 0) {
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &event) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningProgressEventAt(owner, 0U, &event) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(umi_data_server_count(server) == 0U);
        goto cleanup;
    }
    if (strcmp(name, "invalid-arguments") == 0) {
        CHECK(UmiLearningProgressOpen(NULL, "learner", &other) == UMI_STATUS_INVALID_ARGUMENT && other == NULL);
        CHECK(UmiLearningProgressOpen(server, "learner", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressCopySnapshot(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressEventAt(NULL, 0U, &event) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressRecord(owner, NULL, "read", "e1", &event) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressRecord(owner, reader, "read", "e1", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressFindCurrent(owner, NULL, "read", &event) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningProgressRefresh(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        goto cleanup;
    }
    if (strcmp(name, "invalid-tokens") == 0) {
        const char *invalid[] = {"", "../learner", "a/b", "a|b", "a b", "a\nb", "https:x"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
            CHECK(UmiLearningProgressOpen(server, invalid[i], &other) == UMI_STATUS_INVALID_ARGUMENT && other == NULL);
            CHECK(UmiLearningProgressRecord(owner, reader, invalid[i], "e1", &event) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningProgressRecord(owner, reader, "read", invalid[i], &event) == UMI_STATUS_INVALID_ARGUMENT);
        }
        char too_long[UMI_TEACHER_ID_CAPACITY + 1U];
        memset(too_long, 'a', sizeof(too_long)); too_long[sizeof(too_long) - 1U] = '\0';
        CHECK(UmiLearningProgressRecord(owner, reader, too_long, "e1", &event) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiLearningProgressOpen(server, too_long, &other) == UMI_STATUS_CAPACITY_EXCEEDED && other == NULL);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0 && umi_data_server_count(server) == 0U);
        goto cleanup;
    }
    if (strcmp(name, "transaction-busy") == 0) {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressRecord(owner, reader, "read", "e1", &event) == UMI_STATUS_BUSY);
        CHECK(UmiLearningProgressRefresh(owner) == UMI_STATUS_BUSY);
        CHECK(UmiLearningProgressOpen(server, "learner", &other) == UMI_STATUS_BUSY && other == NULL);
        CHECK(umi_data_server_in_transaction(server) && memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
        goto cleanup;
    }
    if (strcmp(name, "wrong-kind") == 0) {
        CHECK(Reader(root, 1U, "example", "example", "progress.fixture", "source", &changed) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressRecord(owner, changed, "read", "e1", &event) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        goto cleanup;
    }
    if (strcmp(name, "stale-writer") == 0)
        CHECK(UmiLearningProgressOpen(server, "learner", &other) == UMI_STATUS_OK);
    CHECK(UmiLearningProgressRecord(owner, reader, "read", "e1", &event) == UMI_STATUS_OK);
    CHECK(event.sequence == 1U && event.pack_revision == 1U && strcmp(event.evidence_reference, "e1") == 0);
    CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK && found.sequence == 1U);
    CHECK(UmiLearningProgressCopySnapshot(owner, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 1U);
    CHECK(!umi_data_server_in_transaction(server));
    if (strcmp(name, "record") == 0) goto cleanup;
    if (strcmp(name, "reopen") == 0 || sqlite_case) {
        UmiLearningProgressClose(owner); owner = NULL;
        if (sqlite_case) {
            umi_data_server_destroy(server); server = NULL;
            UmiStatus opened = strcmp(name, "readonly") == 0 ?
                umi_data_server_open_sqlite_readonly(database, &server) :
                umi_data_server_create_sqlite(database, &server);
            CHECK(opened == UMI_STATUS_OK);
        }
        CHECK(UmiLearningProgressOpen(server, "learner", &owner) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK && found.sequence == 1U);
        if (strcmp(name, "readonly") == 0) {
            memcpy(&guard, &event, sizeof(guard));
            CHECK(UmiLearningProgressRecord(owner, reader, "review", "e2", &event) == UMI_STATUS_PERMISSION_DENIED);
            CHECK(memcmp(&event, &guard, sizeof(event)) == 0 && !umi_data_server_in_transaction(server));
        }
        goto cleanup;
    }
    memcpy(&guard, &event, sizeof(guard));
    if (strcmp(name, "duplicate") == 0) {
        CHECK(UmiLearningProgressRecord(owner, reader, "read", "e1", &event) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
    } else if (strcmp(name, "latest-evidence") == 0) {
        CHECK(UmiLearningProgressRecord(owner, reader, "read", "e2", &event) == UMI_STATUS_OK);
        CHECK(event.sequence == 2U);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK);
        CHECK(strcmp(found.evidence_reference, "e2") == 0 && found.sequence == 2U);
        CHECK(UmiLearningProgressEventAt(owner, 0U, &found) == UMI_STATUS_OK && strcmp(found.evidence_reference, "e1") == 0);
    } else if (strncmp(name, "changed-", 8U) == 0 && strcmp(name, "changed-history") != 0) {
        CHECK(Reader(root, strcmp(name, "changed-revision") == 0 ? 2U : 1U,
            strcmp(name, "changed-digest") == 0 ? "Revised lesson" : "Original lesson", "lesson",
            strcmp(name, "changed-pack") == 0 ? "other.pack" : "progress.fixture",
            strcmp(name, "changed-resource") == 0 ? "other-lesson" : "lesson", &changed) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressFindCurrent(owner, changed, "read", &event) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressRecord(owner, changed, "read", "e1", &event) == UMI_STATUS_OK && event.sequence == 2U);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK && found.sequence == 1U);
    } else if (strcmp(name, "checkpoint") == 0) {
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "review", &event) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
    } else if (strcmp(name, "learner-separation") == 0) {
        CHECK(UmiLearningProgressOpen(server, "learner.2", &other) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressFindCurrent(other, reader, "read", &event) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningProgressRecord(other, reader, "read", "other-evidence", &event) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK);
        CHECK(strcmp(found.evidence_reference, "e1") == 0);
    } else if (strcmp(name, "stale-writer") == 0) {
        CHECK(UmiLearningProgressRecord(other, reader, "review", "e2", &event) == UMI_STATUS_BUSY);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressRefresh(other) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressRecord(other, reader, "review", "e2", &event) == UMI_STATUS_OK && event.sequence == 2U);
        CHECK(UmiLearningProgressCopySnapshot(owner, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 1U);
    } else if (strcmp(name, "changed-history") == 0) {
        CHECK(umi_data_server_get(server, "learning.progress/learner/event/1", wire, sizeof(wire)) == UMI_STATUS_OK);
        char *last = strrchr(wire, '|'); CHECK(last != NULL); last[1] = 'x';
        CHECK(umi_data_server_set(server, "learning.progress/learner/event/1", wire) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressRecord(owner, reader, "review", "e2", &event) == UMI_STATUS_BUSY);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressRefresh(owner) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressFindCurrent(owner, reader, "read", &found) == UMI_STATUS_OK);
        CHECK(strcmp(found.evidence_reference, "x1") == 0);
    } else if (strcmp(name, "capacity") == 0) {
        for (size_t i = 1U; i < UMI_LEARNING_PROGRESS_MAX_EVENTS; ++i) {
            char evidence[32];
            int n = snprintf(evidence, sizeof(evidence), "e%zu", i + 1U);
            CHECK(n > 0 && (size_t)n < sizeof(evidence));
            CHECK(UmiLearningProgressRecord(owner, reader, "read", evidence, &event) == UMI_STATUS_OK);
        }
        memcpy(&guard, &event, sizeof(guard));
        CHECK(UmiLearningProgressRecord(owner, reader, "read", "overflow", &event) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressOpen(server, "learner", &other) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressCopySnapshot(other, &snapshot) == UMI_STATUS_OK && snapshot.event_count == UMI_LEARNING_PROGRESS_MAX_EVENTS);
    } else {
        if (strcmp(name, "corrupt-head") == 0 || strcmp(name, "refresh-invalid") == 0)
            CHECK(umi_data_server_set(server, "learning.progress/learner/head", "LP2|1") == UMI_STATUS_OK);
        else if (strcmp(name, "missing-event") == 0)
            CHECK(umi_data_server_delete(server, "learning.progress/learner/event/1") == UMI_STATUS_OK);
        else if (strcmp(name, "orphan-event") == 0)
            CHECK(umi_data_server_delete(server, "learning.progress/learner/head") == UMI_STATUS_OK);
        else if (strcmp(name, "unknown-record") == 0)
            CHECK(umi_data_server_set(server, "learning.progress/learner/other", "unknown") == UMI_STATUS_OK);
        else if (strcmp(name, "extra-event") == 0)
            CHECK(umi_data_server_set(server, "learning.progress/learner/event/2", "extra") == UMI_STATUS_OK);
        else if (strcmp(name, "duplicate-stored-event") == 0) {
            CHECK(umi_data_server_get(server, "learning.progress/learner/event/1", wire, sizeof(wire)) == UMI_STATUS_OK);
            CHECK(umi_data_server_set(server, "learning.progress/learner/event/2", wire) == UMI_STATUS_OK);
            CHECK(umi_data_server_set(server, "learning.progress/learner/head", "LP1|2") == UMI_STATUS_OK);
        }
        else if (strcmp(name, "malformed-event") == 0)
            CHECK(umi_data_server_set(server, "learning.progress/learner/event/1", "LP1|incomplete") == UMI_STATUS_OK);
        else { result = 2; goto cleanup; }
        CHECK(UmiLearningProgressRefresh(owner) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiLearningProgressRecord(owner, reader, "review", "e2", &event) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&event, &guard, sizeof(event)) == 0);
        CHECK(UmiLearningProgressCopySnapshot(owner, &snapshot) == UMI_STATUS_OK && snapshot.event_count == 1U);
        CHECK(UmiLearningProgressOpen(server, "learner", &other) == UMI_STATUS_PARSE_ERROR && other == NULL);
    }
    CHECK(!umi_data_server_in_transaction(server));
cleanup:
    UmiLearningProgressClose(other);
    UmiLearningProgressClose(owner);
    UmiLearningReaderDestroy(changed);
    UmiLearningReaderDestroy(reader);
    umi_data_server_destroy(server);
    return result;
}
