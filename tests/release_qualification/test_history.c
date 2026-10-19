/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_history.c
 * Purpose: Check persistent identity, progress and stale-entry refusal without launching work.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_history.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, strcmp(name, "large") == 0 ? 5000U : 1U, false) == 0);
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    REQUIRE(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    REQUIRE(UmiJobHistoryCreate(server, "qualification", &history) == UMI_STATUS_OK);
    UmiJobHistoryEntry entry = {0}, out = {0};
    UmiJobIdentity identity;
    REQUIRE(UmiReleaseQualificationJobIdentity(f.run, &identity) == UMI_STATUS_OK);
    REQUIRE(strcmp(identity.subject, f.identity.execution) == 0);
    REQUIRE(strcmp(identity.configuration, f.identity.plan) == 0);
    REQUIRE(strcmp(identity.inputs, UmiReleaseSourceObservationFingerprint(f.before)) == 0);
    if (strcmp(name, "transaction") == 0) {
        REQUIRE(umi_data_server_begin(server) == UMI_STATUS_OK);
        out.id = 999U;
        REQUIRE(UmiReleaseQualificationHistoryBegin(f.run, history, "Scoped check", &out) != UMI_STATUS_OK);
        REQUIRE(out.id == 999U && umi_data_server_in_transaction(server));
        REQUIRE(umi_data_server_rollback(server) == UMI_STATUS_OK);
    }
    REQUIRE(UmiReleaseQualificationHistoryBegin(f.run, history, "Scoped check", &entry) == UMI_STATUS_OK);
    REQUIRE(entry.total_steps == 3U && entry.completed_steps == 0U);
    REQUIRE(UmiJobIdentityCompare(&entry.identity, &identity) == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    if (strcmp(name, "wrong-entry") == 0) {
        UmiJobHistoryEntry other;
        REQUIRE(UmiJobHistoryBegin(history, "build.workflow", "Other", 3U, &other) == UMI_STATUS_OK);
        out.id = 999U;
        REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, other.id, other.revision, &out) == UMI_STATUS_INVALID_STATE);
        REQUIRE(out.id == 999U);
    } else if (strcmp(name, "prepared-success") == 0) {
        REQUIRE(QualificationRecordAll(&f) == 0);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &out) == UMI_STATUS_INVALID_STATE);
    } else {
        uint64_t previous = entry.revision;
        REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
        REQUIRE(entry.state == UMI_JOB_HISTORY_RUNNING);
        if (strcmp(name, "stale") == 0) {
            out.id = 999U;
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, previous, &out) == UMI_STATUS_INVALID_STATE);
            REQUIRE(out.id == 999U);
        } else if (strcmp(name, "repeat") == 0) {
            previous = entry.revision;
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.revision == previous);
        } else if (strcmp(name, "skipped") == 0) {
            UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
            REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_OK);
            result = QualificationResult(UMI_QUALIFICATION_SKIPPED);
            REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.state == UMI_JOB_HISTORY_FAILED && entry.result == UMI_STATUS_INVALID_STATE);
        } else if (strcmp(name, "interrupted-cancel") == 0 || strcmp(name, "interrupted-io") == 0) {
            REQUIRE(QualificationRecordAll(&f) == 0);
            UmiStatus cause = strcmp(name, "interrupted-cancel") == 0 ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR;
            REQUIRE(UmiReleaseQualificationInterrupt(f.run, &f.identity, cause) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.result == cause && entry.completed_steps == 3U);
            REQUIRE(entry.state == (cause == UMI_STATUS_CANCELLED ? UMI_JOB_HISTORY_CANCELLED : UMI_JOB_HISTORY_FAILED));
        } else if (strcmp(name, "abandoned") == 0) {
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, NULL, NULL) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.state == UMI_JOB_HISTORY_FAILED && entry.completed_steps == 0U);
        } else if (strcmp(name, "passed") == 0 || strcmp(name, "large") == 0 ||
                   strcmp(name, "transaction") == 0 || strcmp(name, "late-begin") == 0) {
            REQUIRE(QualificationRecordAll(&f) == 0);
            if (strcmp(name, "late-begin") == 0) {
                out.id = 999U;
                REQUIRE(UmiReleaseQualificationHistoryBegin(f.run, history, "Late", &out) == UMI_STATUS_INVALID_STATE);
                REQUIRE(out.id == 999U);
            }
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.state == UMI_JOB_HISTORY_RUNNING && entry.completed_steps == 2U);
            REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
            REQUIRE(UmiReleaseQualificationHistoryPublish(f.run, history, entry.id, entry.revision, &entry) == UMI_STATUS_OK);
            REQUIRE(entry.state == UMI_JOB_HISTORY_SUCCEEDED && entry.completed_steps == 3U);
        } else REQUIRE(false);
    }
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    REQUIRE(snapshot != NULL);
    REQUIRE(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot->count >= 1U);
    REQUIRE(UmiJobIdentityCompare(&snapshot->entries[0].identity, &identity) == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
    free(snapshot);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    QualificationDestroy(&f);
    return 0;
}
