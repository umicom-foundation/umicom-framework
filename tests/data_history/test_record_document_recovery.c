/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_document_recovery.c
 *
 * PURPOSE:
 *   Persistent-store failures are injected at specific transaction boundaries.
 *   Fixtures contain synthetic bytes and never overwrite an existing database.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Persistent-store failures are injected at specific transaction boundaries.
 * Fixtures contain synthetic bytes and never overwrite an existing database. */
#include "umicom/data/record_document.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiRecordAmendment metadata(void)
{
    UmiRecordAmendment value = {.entity_id = "a", .source = "fictional-import",
        .actor = "test-reviewer", .reason = "Store an example", .recorded_at_nanoseconds = 100U};
    return value;
}
static int failure(const char *name)
{
    UmiDataServer *server = NULL;
    OK(umi_data_server_create_sqlite(":memory:", &server));
    UmiRecordAmendment m = metadata();
    UmiRecordRevision revision;
    OK(umi_record_document_append(server, &m, "original", 8U, 0U, &revision));
    const char *sql = NULL;
    if (strcmp(name, "manifest-failure") == 0) {
        sql = "CREATE TRIGGER reject_write BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-content.%.00' BEGIN SELECT RAISE(ABORT, 'manifest rejected'); END;";
    } else if (strcmp(name, "chunk-failure") == 0) {
        sql = "CREATE TRIGGER reject_write BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-content.%.02' BEGIN SELECT RAISE(ABORT, 'second chunk rejected'); END;";
    } else if (strcmp(name, "head-failure") == 0) {
        sql = "CREATE TRIGGER reject_write BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-history.h.%' BEGIN SELECT RAISE(ABORT, 'head rejected'); END;";
    } else if (strcmp(name, "commit-failure") == 0) {
        sql = "PRAGMA foreign_keys=ON; CREATE TABLE allowed_parent(id INTEGER PRIMARY KEY); CREATE TABLE delayed_reference(id INTEGER REFERENCES allowed_parent(id) DEFERRABLE INITIALLY DEFERRED); CREATE TRIGGER reject_write AFTER INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-content.%.00' BEGIN INSERT INTO delayed_reference VALUES(42); END;";
    } else if (strcmp(name, "automatic-rollback") == 0) {
        sql = "CREATE TRIGGER reject_write BEFORE INSERT ON umicom_kv WHEN NEW.key LIKE 'repo.record-content.%.02' BEGIN SELECT RAISE(ROLLBACK, 'whole transaction aborted'); END;";
    } else { umi_data_server_destroy(server); return 2; }
    OK(umi_data_server_execute(server, sql));
    unsigned char bytes[1025]; memset(bytes, 'z', sizeof bytes);
    unsigned char before[sizeof revision]; memcpy(before, &revision, sizeof revision);
    CHECK(umi_record_document_append(server, &m, bytes, sizeof bytes, 1U, &revision) == UMI_STATUS_IO_ERROR);
    CHECK(memcmp(before, &revision, sizeof revision) == 0);
    CHECK(!umi_data_server_in_transaction(server));
    UmiRecordDocument document;
    OK(umi_record_document_latest(server, "a", &document));
    CHECK(document.record.revision == 1U && document.length == 8U);
    CHECK(memcmp(document.content, "original", 8U) == 0);
    CHECK(umi_record_history_load(server, "a", 2U, &revision) == UMI_STATUS_NOT_FOUND);
    char orphan[32];
    CHECK(umi_data_server_get(server, "repo.record-content.61.00000000000000000002.00", orphan, sizeof orphan) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_data_server_get(server, "repo.record-content.61.00000000000000000002.01", orphan, sizeof orphan) == UMI_STATUS_NOT_FOUND);
    size_t count = 0U; OK(UmiDataServerCountChecked(server, &count)); CHECK(count == 4U);
    OK(umi_data_server_execute(server, "DROP TRIGGER reject_write;"));
    OK(umi_record_document_append(server, &m, bytes, sizeof bytes, 1U, &revision)); CHECK(revision.revision == 2U);
    uint64_t revisions = 0U; OK(umi_record_document_verify(server, "a", &revisions)); CHECK(revisions == 2U);
    umi_data_server_destroy(server);
    return 0;
}
static int reopen(const char *directory)
{
    char path[1024];
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    int length = snprintf(path, sizeof path, "%s/document-%lu-%llu.sqlite", directory,
        process, (unsigned long long)time(NULL));
    CHECK(length > 0 && (size_t)length < sizeof path);
    FILE *reserved = fopen(path, "wbx"); CHECK(reserved != NULL); CHECK(fclose(reserved) == 0);
    UmiDataServer *server = NULL;
    OK(umi_data_server_create_sqlite(path, &server));
    UmiRecordAmendment m = metadata();
    UmiRecordRevision revision;
    const unsigned char original[] = {0U, 255U, '\n', 'a'};
    OK(umi_record_document_append(server, &m, original, sizeof original, 0U, &revision));
    umi_data_server_destroy(server); server = NULL;
    OK(umi_data_server_create_sqlite(path, &server));
    UmiRecordDocument document;
    OK(umi_record_document_latest(server, "a", &document));
    CHECK(document.length == sizeof original && memcmp(document.content, original, sizeof original) == 0);
    OK(umi_record_document_append(server, &m, "reviewed", 8U, 1U, &revision));
    umi_data_server_destroy(server); server = NULL;
    OK(umi_data_server_create_sqlite(path, &server));
    OK(umi_record_document_load(server, "a", 1U, &document)); CHECK(document.length == sizeof original);
    OK(umi_record_document_latest(server, "a", &document)); CHECK(document.record.revision == 2U);
    uint64_t count = 0U; OK(umi_record_document_verify(server, "a", &count)); CHECK(count == 2U);
    umi_data_server_destroy(server); CHECK(remove(path) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "reopen") == 0) return reopen(argv[2]);
    return argc == 2 ? failure(argv[1]) : 2;
}
