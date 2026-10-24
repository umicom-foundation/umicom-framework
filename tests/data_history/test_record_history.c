/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic metadata exercises the shared storage boundary. No care decisions,
 * payments, external identities or external provider traffic are involved. */
#include "umicom/data/record_history.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiRecordAmendment draft(const char *entity)
{
    UmiRecordAmendment a = {.source = "synthetic-fixture", .actor = "fictional-reviewer",
        .reason = "Record an example observation", .observed_at_nanoseconds = 10U,
        .recorded_at_nanoseconds = 20U};
    (void)snprintf(a.entity_id, sizeof a.entity_id, "%s", entity);
    /* Known SHA-256 of the empty byte string: fixture payload is empty. */
    strcpy(a.content_sha256, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    return a;
}
typedef struct HistoryWorker {
    UmiDataServer *server;
    UmiStatus status;
} HistoryWorker;
static void append_from_worker(HistoryWorker *work)
{
    UmiRecordAmendment a = draft("worker");
    UmiRecordRevision result;
    work->status = umi_record_history_append(work->server, &a, 0U, &result);
}
#ifdef _WIN32
static DWORD WINAPI worker_entry(LPVOID context) { append_from_worker(context); return 0U; }
static int join_worker(HistoryWorker *work)
{
    HANDLE thread = CreateThread(NULL, 0, worker_entry, work, 0, NULL);
    if (thread == NULL) { return 1; }
    DWORD result = WaitForSingleObject(thread, INFINITE);
    (void)CloseHandle(thread);
    return result == WAIT_OBJECT_0 ? 0 : 1;
}
#else
static void *worker_entry(void *context) { append_from_worker(context); return NULL; }
static int join_worker(HistoryWorker *work)
{
    pthread_t thread;
    if (pthread_create(&thread, NULL, worker_entry, work) != 0) { return 1; }
    return pthread_join(thread, NULL);
}
#endif
static int exercise(UmiDataServer *server, const char *name)
{
    UmiRecordAmendment a = draft("a");
    UmiRecordRevision record, old;
    if (strcmp(name, "roundtrip") == 0) {
        const char *entities[] = {"synthetic-care-observation", "synthetic-bank-import"};
        for (size_t i = 0U; i < 2U; ++i) {
            a = draft(entities[i]);
            OK(umi_record_history_append(server, &a, 0U, &record));
            CHECK(record.revision == 1U && record.previous_revision == 0U);
            strcpy(a.source, "reviewed-import"); strcpy(a.actor, "second-fictional-reviewer");
            strcpy(a.reason, "Correct the example source label");
            a.recorded_at_nanoseconds = 30U;
            OK(umi_record_history_append(server, &a, 1U, &record));
            OK(umi_record_history_load(server, a.entity_id, 1U, &old));
            CHECK(strcmp(old.amendment.source, "synthetic-fixture") == 0);
            CHECK(strcmp(old.amendment.actor, "fictional-reviewer") == 0);
            OK(umi_record_history_latest(server, a.entity_id, &record));
            CHECK(record.revision == 2U && record.previous_revision == 1U);
            uint64_t revisions = 0U; OK(umi_record_history_verify(server, a.entity_id, &revisions));
            CHECK(revisions == 2U);
        }
    } else if (strcmp(name, "stale") == 0) {
        OK(umi_record_history_append(server, &a, 0U, &record));
        unsigned char before[sizeof record]; memcpy(before, &record, sizeof record);
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_history_append(server, &a, 2U, &record) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, &record, sizeof record) == 0);
        OK(umi_record_history_latest(server, "a", &old)); CHECK(old.revision == 1U);
        CHECK(!umi_data_server_in_transaction(server));
    } else if (strcmp(name, "nested") == 0) {
        OK(umi_data_server_begin(server));
        OK(umi_data_server_set(server, "caller-owned", "pending"));
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_BUSY);
        uint64_t count = 99U;
        CHECK(umi_record_history_verify(server, "a", &count) == UMI_STATUS_BUSY);
        CHECK(count == 99U && umi_data_server_in_transaction(server));
        OK(umi_data_server_commit(server));
        char text[16]; OK(umi_data_server_get(server, "caller-owned", text, sizeof text));
        CHECK(strcmp(text, "pending") == 0);
    } else if (strcmp(name, "competing") == 0) {
        HistoryWorker work = {.server = server, .status = UMI_STATUS_INTERNAL_ERROR};
        OK(umi_data_server_begin(server));
        CHECK(join_worker(&work) == 0); CHECK(work.status == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server));
        OK(umi_data_server_rollback(server));
        CHECK(join_worker(&work) == 0); CHECK(work.status == UMI_STATUS_OK);
        OK(umi_record_history_latest(server, "worker", &record)); CHECK(record.revision == 1U);
    } else if (strcmp(name, "isolation") == 0) {
        const char *names[] = {"a.b", "a/b", "a:b", "61", "a"};
        for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i) {
            a = draft(names[i]); OK(umi_record_history_append(server, &a, 0U, &record));
            OK(umi_record_history_latest(server, names[i], &old));
            CHECK(strcmp(old.amendment.entity_id, names[i]) == 0);
        }
        memset(a.entity_id, 'z', sizeof a.entity_id - 1U); a.entity_id[63] = '\0';
        OK(umi_record_history_append(server, &a, 0U, &record));
        OK(umi_record_history_latest(server, a.entity_id, &record));
    } else if (strcmp(name, "invalid") == 0) {
        memset(&record, 0xa5, sizeof record);
        unsigned char before[sizeof record]; memcpy(before, &record, sizeof record);
        a.actor[0] = '\0';
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        a = draft("a"); strcpy(a.reason, "bad\tfield");
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        a = draft("a"); memset(a.source, 's', sizeof a.source);
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        a = draft("a"); a.content_sha256[0] = 'G';
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        a = draft("a"); a.recorded_at_nanoseconds = 0U;
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        a = draft("a");
        CHECK(umi_record_history_append(server, &a, UINT64_MAX, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, &record, sizeof record) == 0);
        size_t count = 99U; OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 0U);
    } else if (strcmp(name, "clock") == 0) {
        OK(umi_record_history_append(server, &a, 0U, &record));
        a.recorded_at_nanoseconds = 19U;
        CHECK(umi_record_history_append(server, &a, 1U, &record) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(record.revision == 1U);
        a.recorded_at_nanoseconds = 20U;
        OK(umi_record_history_append(server, &a, 1U, &record));
    } else if (strcmp(name, "orphan") == 0) {
        OK(umi_data_server_set(server, "repo.record-history.r.61.00000000000000000001", "unclaimed"));
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_INVALID_STATE);
        char text[32]; OK(umi_data_server_get(server, "repo.record-history.r.61.00000000000000000001", text, sizeof text));
        CHECK(strcmp(text, "unclaimed") == 0);
        CHECK(umi_record_history_latest(server, "a", &record) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "corrupt") == 0) {
        OK(umi_record_history_append(server, &a, 0U, &record));
        OK(umi_record_history_append(server, &a, 1U, &record));
        OK(umi_data_server_delete(server, "repo.record-history.r.61.00000000000000000001"));
        uint64_t revisions = 99U;
        CHECK(umi_record_history_verify(server, "a", &revisions) == UMI_STATUS_NOT_FOUND);
        CHECK(revisions == 99U && !umi_data_server_in_transaction(server));
        OK(umi_data_server_set(server, "repo.record-history.h.61", "2junk"));
        CHECK(umi_record_history_latest(server, "a", &record) == UMI_STATUS_PARSE_ERROR);
        CHECK(umi_record_history_append(server, &a, 2U, &record) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "codec") == 0) {
        record.amendment = a; record.revision = UINT64_MAX; record.previous_revision = UINT64_MAX - 1U;
        record.amendment.observed_at_nanoseconds = UINT64_MAX;
        record.amendment.recorded_at_nanoseconds = UINT64_MAX;
        char text[UMI_RECORD_REVISION_TEXT_CAPACITY];
        OK(umi_record_revision_format(&record, text, sizeof text));
        OK(umi_record_revision_parse(text, strlen(text), &old));
        CHECK(old.revision == UINT64_MAX && old.amendment.recorded_at_nanoseconds == UINT64_MAX);
        char small[8] = "kept";
        CHECK(umi_record_revision_format(&record, small, sizeof small) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(small, "kept") == 0);
        unsigned char before[sizeof old]; memcpy(before, &old, sizeof old);
        text[4] = '9';
        CHECK(umi_record_revision_parse(text, strlen(text), &old) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, &old, sizeof old) == 0);
        OK(umi_record_revision_format(&record, text, sizeof text));
        size_t length = strlen(text); text[8] = '\0';
        CHECK(umi_record_revision_parse(text, length, &old) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(before, &old, sizeof old) == 0);
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(umi_data_server_backend(server) == UMI_DATA_BACKEND_MEMORY);
        /* Leave one slot: row insertion succeeds, head insertion must fail.
         * Verify that the owned transaction removes the provisional row. */
        for (unsigned i = 0U; i < 2047U; ++i) {
            char key[32]; (void)snprintf(key, sizeof key, "occupied-%u", i);
            OK(umi_data_server_set(server, key, "fixture"));
        }
        memset(&record, 0xa5, sizeof record);
        unsigned char before[sizeof record]; memcpy(before, &record, sizeof record);
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, &record, sizeof record) == 0);
        CHECK(umi_record_history_load(server, "a", 1U, &record) == UMI_STATUS_NOT_FOUND);
        CHECK(!umi_data_server_in_transaction(server));
        size_t count = 0U; OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 2047U);
    } else if (strcmp(name, "head-failure") == 0) {
        CHECK(umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE);
        OK(umi_data_server_execute(server, "CREATE TRIGGER reject_head BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-history.h.%' BEGIN SELECT RAISE(ABORT, 'fixture rejection'); END;"));
        memset(&record, 0xa5, sizeof record);
        unsigned char before[sizeof record]; memcpy(before, &record, sizeof record);
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_IO_ERROR);
        CHECK(memcmp(before, &record, sizeof record) == 0);
        CHECK(umi_record_history_load(server, "a", 1U, &record) == UMI_STATUS_NOT_FOUND);
        CHECK(!umi_data_server_in_transaction(server));
        OK(umi_data_server_execute(server, "DROP TRIGGER reject_head;"));
        OK(umi_record_history_append(server, &a, 0U, &record)); CHECK(record.revision == 1U);
    } else if (strcmp(name, "commit-failure") == 0) {
        CHECK(umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE);
        /* A deferred foreign-key violation fails at COMMIT after both history
         * writes succeeded. Rollback must remove the entire provisional edit. */
        OK(umi_data_server_execute(server, "PRAGMA foreign_keys=ON; CREATE TABLE allowed_parent(id INTEGER PRIMARY KEY); CREATE TABLE delayed_reference(id INTEGER REFERENCES allowed_parent(id) DEFERRABLE INITIALLY DEFERRED); CREATE TRIGGER reject_commit AFTER INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-history.h.%' BEGIN INSERT INTO delayed_reference VALUES(42); END;"));
        memset(&record, 0xa5, sizeof record);
        unsigned char before[sizeof record]; memcpy(before, &record, sizeof record);
        CHECK(umi_record_history_append(server, &a, 0U, &record) == UMI_STATUS_IO_ERROR);
        CHECK(memcmp(before, &record, sizeof record) == 0);
        CHECK(!umi_data_server_in_transaction(server));
        CHECK(umi_record_history_latest(server, "a", &record) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_record_history_load(server, "a", 1U, &record) == UMI_STATUS_NOT_FOUND);
        OK(umi_data_server_execute(server, "DROP TRIGGER reject_commit;"));
        OK(umi_record_history_append(server, &a, 0U, &record)); CHECK(record.revision == 1U);
    } else {
        return 2;
    }
    return 0;
}

static int reopen(const char *directory)
{
    char file[1024];
    int written = snprintf(file, sizeof file, "%s/history-%llu-%llu.sqlite", directory,
        (unsigned long long)time(NULL), (unsigned long long)clock());
    CHECK(written > 0 && (size_t)written < sizeof file);
    /* Exclusive creation never overwrites another run's fixture or user data. */
    FILE *reserved = fopen(file, "wbx"); CHECK(reserved != NULL); CHECK(fclose(reserved) == 0);
    UmiDataServer *server = NULL;
    OK(umi_data_server_create_sqlite(file, &server));
    UmiRecordAmendment a = draft("synthetic-persistent-record");
    UmiRecordRevision record;
    OK(umi_record_history_append(server, &a, 0U, &record));
    umi_data_server_destroy(server); server = NULL;
    OK(umi_data_server_create_sqlite(file, &server));
    OK(umi_record_history_latest(server, a.entity_id, &record)); CHECK(record.revision == 1U);
    OK(umi_record_history_append(server, &a, 1U, &record)); CHECK(record.revision == 2U);
    uint64_t count = 0U; OK(umi_record_history_verify(server, a.entity_id, &count)); CHECK(count == 2U);
    umi_data_server_destroy(server);
    CHECK(remove(file) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc < 3) { return 2; }
    if (strcmp(argv[1], "sqlite") == 0 && strcmp(argv[2], "reopen") == 0) {
        return argc == 4 ? reopen(argv[3]) : 2;
    }
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (strcmp(argv[1], "memory") == 0) {
        status = umi_data_server_create_memory(&server);
    } else if (strcmp(argv[1], "sqlite") == 0) {
        status = umi_data_server_create_sqlite(":memory:", &server);
    } else { return 2; }
    CHECK(status == UMI_STATUS_OK);
    int result = exercise(server, argv[2]);
    umi_data_server_destroy(server);
    return result;
}
