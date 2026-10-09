/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_sqlite.c
 * PURPOSE: Prove archive commit, reopen and failure rollback through SQLite fixtures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "fixture.h"
#include "umicom/testing/archive_compare.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
#ifndef UMICOM_HAS_SQLITE
    (void)argv;
    return 77;
#else
    const char *name = argv[1];
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "tests-caf\xc3\xa9.sqlite");
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(true);
    /* Exercise durable context in success, rollback, contention and reopen cases. */
    memset(origin.identity.subject, 'a', 64);
    memset(origin.identity.configuration, 'b', 64);
    memset(origin.identity.inputs, 'c', 64);
    UmiTestArchiveEntry entry = {0};
    entry.id = 999;
    if (strcmp(name, "write-failure") == 0 || strcmp(name, "commit-failure") == 0)
    {
        if (strcmp(name, "commit-failure") == 0)
        {
            CHECK(umi_data_server_execute(server, "PRAGMA foreign_keys=ON;") == UMI_STATUS_OK);
            CHECK(umi_data_server_execute(server, "CREATE TABLE parent(id INTEGER PRIMARY KEY);") ==
                  UMI_STATUS_OK);
            CHECK(
                umi_data_server_execute(
                    server,
                    "CREATE TABLE child(id INTEGER REFERENCES parent(id) DEFERRABLE INITIALLY DEFERRED);") ==
                UMI_STATUS_OK);
            CHECK(umi_data_server_execute(
                      server, "CREATE TEMP TRIGGER fail_save AFTER INSERT ON umicom_kv WHEN "
                              "NEW.key='ctest-archive/test/next' BEGIN INSERT INTO child VALUES(9); END;") ==
                  UMI_STATUS_OK);
        }
        else
            CHECK(
                umi_data_server_execute(
                    server,
                    "CREATE TEMP TRIGGER fail_save BEFORE INSERT ON umicom_kv WHEN "
                    "NEW.key='ctest-archive/test/next' BEGIN SELECT RAISE(ABORT,'fixture refusal'); END;") ==
                UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) != UMI_STATUS_OK && entry.id == 999);
        CHECK(!umi_data_server_in_transaction(server));
        FixtureEmpty(archive);
        CHECK(umi_data_server_count(server) == 0);
        CHECK(umi_data_server_execute(server, "DROP TRIGGER fail_save;") == UMI_STATUS_OK);
    }
    else if (strcmp(name, "busy") == 0)
    {
        UmiDataServer *other = NULL;
        UmiTestArchive *reader = NULL;
        CHECK(umi_data_server_create_sqlite(path, &other) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveCreate(other, "test", &reader) == UMI_STATUS_OK);
        CHECK(umi_data_server_begin(other) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_BUSY && entry.id == 999);
        UmiTestArchiveComparison *comparison = NULL;
        CHECK(UmiTestArchiveCompare(archive, 1, 1, NULL, &comparison) == UMI_STATUS_BUSY);
        CHECK(comparison == NULL);
        CHECK(umi_data_server_in_transaction(other));
        CHECK(umi_data_server_rollback(other) == UMI_STATUS_OK);
        UmiTestArchiveDestroy(reader);
        umi_data_server_destroy(other);
    }
    else
        CHECK(strcmp(name, "reopen") == 0 || strcmp(name, "remove-failure") == 0);
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK && entry.id == 1);
    if (strcmp(name, "remove-failure") == 0)
    {
        CHECK(umi_data_server_execute(
                  server,
                  "CREATE TEMP TRIGGER refuse_remove BEFORE DELETE ON umicom_kv WHEN "
                  "OLD.key='ctest-archive/test/slot/0' BEGIN SELECT RAISE(ABORT,'fixture refusal'); END;") ==
              UMI_STATUS_OK);
        CHECK(UmiTestArchiveRemove(archive, 1) != UMI_STATUS_OK);
        CHECK(!umi_data_server_in_transaction(server));
    }
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiTestResult *result = calloc(1, sizeof(*result));
    CHECK(result != NULL);
    uint32_t attempt = 0;
    CHECK(UmiTestArchiveResultAt(archive, 1, 2, result, &attempt) == UMI_STATUS_OK &&
          result->state == UMI_TEST_STATE_FAILED && attempt == 2 &&
          strcmp(result->output, job->results[2].output) == 0);
    /* Reopened comparisons validate durable rows, then own copies independent
     * of later archive removal. Both selected tests retain failure evidence. */
    UmiTestArchiveComparison *comparison = NULL;
    UmiTestArchiveComparisonSummary compared = {0};
    CHECK(UmiTestArchiveCompare(archive, 1, 1, NULL, &comparison) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveComparisonRead(comparison, &compared) == UMI_STATUS_OK &&
          compared.counts[UMI_TEST_ARCHIVE_PERSISTING_FAILURE] == 2);
    CHECK(compared.identity_comparison == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    CHECK(compared.same_recorded_selection && compared.selection_evidence_recorded);
    CHECK(memcmp(&compared.baseline.origin.identity, &origin.identity, sizeof(origin.identity)) == 0);
    CHECK(UmiTestArchiveRemove(archive, 1) == UMI_STATUS_OK);
    UmiTestArchiveComparisonRow copied = {0};
    CHECK(UmiTestArchiveComparisonRowAt(comparison, 0, &copied) == UMI_STATUS_OK &&
          copied.before.failed == 1 && copied.after.failed == 1);
    UmiTestArchiveComparisonDestroy(comparison);
    FixtureEmpty(archive);
    CHECK(umi_data_server_count(server) == 1);
    free(result);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
#endif
}
