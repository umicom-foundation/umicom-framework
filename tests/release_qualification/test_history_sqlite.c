/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_history_sqlite.c
 * Purpose: Verify durable identities and transaction failure without replaying work.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#include "../build_log/fixture.h"
#include "umicom/distribution/runtime/qualification_history.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    bool unfinished = strcmp(argv[1], "unfinished") == 0;
    bool fault = strcmp(argv[1], "commit-failure") == 0;
    REQUIRE(unfinished || fault || strcmp(argv[1], "reopen") == 0);
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "qualification.sqlite");
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 1U, false) == 0);
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    REQUIRE(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    REQUIRE(UmiJobHistoryCreate(server, "qualification", &history) == UMI_STATUS_OK);
    UmiJobHistoryEntry entry = {0};
    UmiJobIdentity identity;
    REQUIRE(UmiReleaseQualificationJobIdentity(f.run, &identity) == UMI_STATUS_OK);
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    REQUIRE(snapshot != NULL);
    if (fault) {
        REQUIRE(umi_data_server_execute(server, "PRAGMA foreign_keys=ON;") == UMI_STATUS_OK);
        REQUIRE(umi_data_server_execute(server, "CREATE TABLE parent(id INTEGER PRIMARY KEY);") == UMI_STATUS_OK);
        REQUIRE(umi_data_server_execute(server,
            "CREATE TABLE child(parent INTEGER REFERENCES parent(id) DEFERRABLE INITIALLY DEFERRED);") == UMI_STATUS_OK);
        REQUIRE(umi_data_server_execute(server,
            "CREATE TEMP TRIGGER reject_qualification AFTER INSERT ON umicom_kv "
            "WHEN NEW.key='job-history/qualification/next' BEGIN INSERT INTO child VALUES(99); END;") == UMI_STATUS_OK);
        entry.id = 999U;
        REQUIRE(UmiReleaseQualificationHistoryBegin(f.run, history, "Scoped check", &entry) != UMI_STATUS_OK);
        REQUIRE(entry.id == 999U);
        REQUIRE(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 0U);
        REQUIRE(umi_data_server_execute(server, "DROP TRIGGER reject_qualification;") == UMI_STATUS_OK);
    }
    REQUIRE(UmiReleaseQualificationHistoryBegin(f.run, history, "Scoped check", &entry) == UMI_STATUS_OK);
    REQUIRE(entry.id == 1U);
    REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
    if (!unfinished) {
        REQUIRE(QualificationRecordAll(&f) == 0);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
    }
    UmiJobHistoryDestroy(history); history = NULL;
    umi_data_server_destroy(server); server = NULL;
    QualificationDestroy(&f);
    REQUIRE(umi_data_server_create_sqlite(path, &server) == UMI_STATUS_OK);
    REQUIRE(UmiJobHistoryCreate(server, "qualification", &history) == UMI_STATUS_OK);
    REQUIRE(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot->count == 1U && snapshot->unfinished_count == (unfinished ? 1U : 0U));
    REQUIRE(UmiJobIdentityCompare(&snapshot->entries[0].identity, &identity) == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    REQUIRE(snapshot->entries[0].state == (unfinished ? UMI_JOB_HISTORY_RUNNING : UMI_JOB_HISTORY_SUCCEEDED));
    REQUIRE(snapshot->entries[0].completed_steps == (unfinished ? 0U : 3U));
    /* Reopening only reads evidence; it never reconstructs or resumes the run. */
    REQUIRE(snapshot->entries[0].revision == entry.revision);
    free(snapshot);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    return 0;
}
