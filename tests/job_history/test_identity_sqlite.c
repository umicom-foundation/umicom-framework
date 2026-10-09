/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_identity_sqlite.c
 * PURPOSE: Verify identity survives reopen and remains atomic when SQLite refuses a commit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/build/job_history.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
#ifndef UMICOM_HAS_SQLITE
    (void)argv;
    return 77;
#else
    const bool fault = strcmp(argv[1], "commit-failure") == 0;
    CHECK(fault || strcmp(argv[1], "reopen") == 0 || strcmp(argv[1], "mixed") == 0);
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "identity.sqlite");
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    CHECK(snapshot != NULL);
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    UmiJobIdentity identity;
    CHECK(UmiBuildProfileJobIdentity(&profile, &identity) == UMI_STATUS_OK);
    UmiJobHistoryEntry entry = {0}, sentinel = {0};
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "build", &history) == UMI_STATUS_OK);
    if (fault)
    {
        /* Failure occurs at transaction commit, after the identity row was
         * written. No partial row, consumed ID or successful output may escape. */
        CHECK(umi_data_server_execute(server, "PRAGMA foreign_keys=ON;") == UMI_STATUS_OK);
        CHECK(umi_data_server_execute(server, "CREATE TABLE parent(id INTEGER PRIMARY KEY);") ==
              UMI_STATUS_OK);
        CHECK(umi_data_server_execute(server,
                                      "CREATE TABLE child(parent INTEGER REFERENCES parent(id) "
                                      "DEFERRABLE INITIALLY DEFERRED);") == UMI_STATUS_OK);
        CHECK(umi_data_server_execute(
                  server,
                  "CREATE TEMP TRIGGER fail_identity AFTER INSERT ON umicom_kv WHEN "
                  "NEW.key='job-history/build/next' BEGIN INSERT INTO child VALUES(99); END;") ==
              UMI_STATUS_OK);
        memset(&sentinel, 0x5a, sizeof(sentinel));
        memcpy(&entry, &sentinel, sizeof(entry));
        CHECK(UmiJobHistoryBeginIdentified(history, "build", "Build", 1U, &identity, &entry) !=
              UMI_STATUS_OK);
        CHECK(memcmp(&entry, &sentinel, sizeof(entry)) == 0);
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 0U);
        CHECK(umi_data_server_execute(server, "DROP TRIGGER fail_identity;") == UMI_STATUS_OK);
    }
    if (strcmp(argv[1], "mixed") == 0)
        CHECK(UmiJobHistoryBegin(history, "build", "Legacy unfinished", 1U, &entry) ==
              UMI_STATUS_OK);
    CHECK(UmiJobHistoryBeginIdentified(history, "build", "Identified build", 1U, &identity,
                                       &entry) == UMI_STATUS_OK);
    CHECK(entry.id == (strcmp(argv[1], "mixed") == 0 ? 2U : 1U));
    CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0U,
                              UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 1U,
                              UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    history = NULL;
    server = NULL;
    CHECK(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "build", &history) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK);
    CHECK(snapshot->count == (strcmp(argv[1], "mixed") == 0 ? 2U : 1U));
    CHECK(UmiJobIdentityCompare(&snapshot->entries[snapshot->count - 1U].identity, &identity) ==
          UMI_JOB_IDENTITY_INPUTS_UNRECORDED);
    if (strcmp(argv[1], "mixed") == 0)
    {
        CHECK(!UmiJobIdentityIsRecorded(&snapshot->entries[0].identity));
        size_t removed = 0U;
        CHECK(UmiJobHistoryPruneFinished(history, &removed) == UMI_STATUS_OK && removed == 1U);
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 1U);
        CHECK(snapshot->entries[0].id == 1U && snapshot->unfinished_count == 1U);
    }
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(snapshot);
    return 0;
#endif
}
