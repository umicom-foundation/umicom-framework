/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_sqlite.c
 * PURPOSE: Check reopen, competing connections and rollback after an actual SQLite write failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/data/job_history.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
#ifndef UMICOM_HAS_SQLITE
    (void)argv;
    return 77;
#else
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "jobs.sqlite");
    UmiDataServer *server = NULL, *other_server = NULL;
    UmiJobHistory *history = NULL, *other = NULL;
    UmiJobHistoryEntry entry = {0}, unchanged = {0};
    UmiJobHistorySnapshot *snapshot = calloc(1, sizeof(*snapshot));
    CHECK(snapshot != NULL);
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "test", &history) == UMI_STATUS_OK);
    if (strcmp(argv[1], "rollback") == 0 || strcmp(argv[1], "commit-failure") == 0)
    {
        bool commit_failure = strcmp(argv[1], "commit-failure") == 0;
        if (commit_failure)
        {
            CHECK(umi_data_server_execute(server, "PRAGMA foreign_keys=ON;") == UMI_STATUS_OK);
            CHECK(umi_data_server_execute(server, "CREATE TABLE fixture_parent(id INTEGER PRIMARY KEY);") ==
                  UMI_STATUS_OK);
            CHECK(umi_data_server_execute(server, "CREATE TABLE fixture_child(parent INTEGER REFERENCES "
                                                  "fixture_parent(id) DEFERRABLE INITIALLY DEFERRED);") ==
                  UMI_STATUS_OK);
            CHECK(umi_data_server_execute(server,
                                          "CREATE TEMP TRIGGER block_job_metadata AFTER INSERT ON umicom_kv "
                                          "WHEN NEW.key='job-history/test/next' BEGIN INSERT INTO "
                                          "fixture_child VALUES(99); END;") == UMI_STATUS_OK);
        }
        else
            CHECK(umi_data_server_execute(server,
                                          "CREATE TEMP TRIGGER block_job_metadata BEFORE INSERT ON umicom_kv "
                                          "WHEN NEW.key='job-history/test/next' BEGIN SELECT "
                                          "RAISE(ABORT,'fixture refusal'); END;") == UMI_STATUS_OK);
        unchanged.id = 99;
        CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &unchanged) != UMI_STATUS_OK &&
              unchanged.id == 99);
        CHECK(!umi_data_server_in_transaction(server));
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 0);
        CHECK(umi_data_server_execute(server, "DROP TRIGGER block_job_metadata;") == UMI_STATUS_OK);
        CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &entry) == UMI_STATUS_OK && entry.id == 1);
    }
    else
    {
        CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &entry) == UMI_STATUS_OK);
        CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0,
                                  UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
        if (strcmp(argv[1], "reopen") == 0)
            CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 1,
                                      UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
        if (strcmp(argv[1], "busy") == 0 || strcmp(argv[1], "connections") == 0)
        {
            CHECK(umi_data_server_create_sqlite(path, &other_server) == UMI_STATUS_OK);
            CHECK(UmiJobHistoryCreate(other_server, "test", &other) == UMI_STATUS_OK);
            if (strcmp(argv[1], "busy") == 0)
            {
                CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
                CHECK(UmiJobHistoryBegin(other, "build", "Other", 1, &unchanged) == UMI_STATUS_BUSY);
                CHECK(umi_data_server_in_transaction(server));
                CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
            }
            CHECK(UmiJobHistoryUpdate(other, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 1,
                                      UMI_STATUS_OK, &unchanged) == UMI_STATUS_OK);
            CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 1,
                                      UMI_STATUS_OK, &entry) == UMI_STATUS_BUSY);
        }
        else
            CHECK(strcmp(argv[1], "reopen") == 0 || strcmp(argv[1], "unfinished") == 0);
    }
    UmiJobHistoryDestroy(other);
    umi_data_server_destroy(other_server);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    history = NULL;
    server = NULL;
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "test", &history) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 1);
    CHECK(snapshot->unfinished_count == (strcmp(argv[1], "reopen") == 0 ? 0U : 1U));
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(snapshot);
    return 0;
#endif
}
