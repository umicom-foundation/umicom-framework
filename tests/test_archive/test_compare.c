/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_compare.c
 * PURPOSE: Verify outcome semantics, identity matching and atomic rejection of corrupt evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/testing/archive_compare.h"

/* Derive metadata from the controlled attempts so each scenario differs in
 * evidence, rather than bypassing archive validation with impossible counts. */
static void Recount(UmiCtestJob *job, size_t completed)
{
    UmiCtestJobSnapshot total = {0};
    total.task_id = 91;
    total.planned = job->plan.request_count * job->plan.repeat_count;
    total.completed = completed;
    for (size_t i = 0; i < completed; ++i)
    {
        UmiTestResult *result = &job->results[i];
        switch (result->state)
        {
        case UMI_TEST_STATE_PASSED:
            ++total.passed;
            break;
        case UMI_TEST_STATE_FAILED:
            ++total.failed;
            break;
        case UMI_TEST_STATE_SKIPPED:
            ++total.skipped;
            break;
        case UMI_TEST_STATE_CANCELLED:
            ++total.cancelled;
            break;
        case UMI_TEST_STATE_TIMED_OUT:
            ++total.timed_out;
            break;
        default:
            ++total.not_run;
            break;
        }
        if (total.first_error == UMI_STATUS_OK && result->status != UMI_STATUS_OK)
            total.first_error = result->status;
        total.duration_ms = UINT64_MAX - total.duration_ms < result->duration_ms
                                ? UINT64_MAX
                                : total.duration_ms + result->duration_ms;
    }
    total.status = total.first_error;
    total.state = total.status == UMI_STATUS_OK          ? UMI_TASK_SUCCEEDED
                  : total.status == UMI_STATUS_CANCELLED ? UMI_TASK_CANCELLED
                                                         : UMI_TASK_FAILED;
    job->snapshot = total;
}
static void SetOutcome(UmiCtestJob *job, size_t index, UmiTestState state, UmiStatus status)
{
    job->results[index].state = state;
    job->results[index].status = status;
    job->results[index].exit_code = status == UMI_STATUS_OK ? 0 : -1;
}
static void PassAll(UmiCtestJob *job)
{
    for (size_t i = 0; i < 4U; ++i)
        SetOutcome(job, i, UMI_TEST_STATE_PASSED, UMI_STATUS_OK);
    Recount(job, 4);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    PassAll(job);
    UmiTestArchiveOrigin origin = FixtureOrigin(false);
    if (strncmp(name, "identity-", 9) == 0)
    {
        memset(origin.identity.subject, 'a', 64);
        memset(origin.identity.configuration, 'b', 64);
        if (strcmp(name, "identity-missing-inputs") != 0)
            memset(origin.identity.inputs, 'c', 64);
    }
    UmiTestArchiveEntry before = {0}, after = {0};
    if (strcmp(name, "recovered") == 0 || strcmp(name, "persisting") == 0)
    {
        SetOutcome(job, 0, UMI_TEST_STATE_FAILED, UMI_STATUS_IO_ERROR);
        Recount(job, 4);
    }
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &before) == UMI_STATUS_OK);
    PassAll(job);
    UmiTestArchiveChange expected = UMI_TEST_ARCHIVE_UNCHANGED;
    size_t expected_count = 2, completed = 4;
    UmiStatus expected_status = UMI_STATUS_OK;
    if (strcmp(name, "regression") == 0 || strcmp(name, "mixed") == 0 || strcmp(name, "persisting") == 0 ||
        strcmp(name, "timeout") == 0 || strcmp(name, "report-error") == 0)
    {
        UmiTestState outcome = strcmp(name, "timeout") == 0        ? UMI_TEST_STATE_TIMED_OUT
                               : strcmp(name, "report-error") == 0 ? UMI_TEST_STATE_SKIPPED
                                                                   : UMI_TEST_STATE_FAILED;
        SetOutcome(job, 0, outcome, UMI_STATUS_IO_ERROR);
        expected = strcmp(name, "persisting") == 0 ? UMI_TEST_ARCHIVE_PERSISTING_FAILURE
                                                   : UMI_TEST_ARCHIVE_REGRESSION;
        expected_count = 1;
    }
    else if (strcmp(name, "recovered") == 0)
    {
        expected = UMI_TEST_ARCHIVE_RECOVERED;
        expected_count = 1;
    }
    else if (strcmp(name, "skipped") == 0)
    {
        SetOutcome(job, 0, UMI_TEST_STATE_SKIPPED, UMI_STATUS_OK);
        expected = UMI_TEST_ARCHIVE_OUTCOME_CHANGED;
        expected_count = 1;
    }
    else if (strcmp(name, "cancelled") == 0 || strcmp(name, "not-run") == 0)
    {
        SetOutcome(job, 0, strcmp(name, "cancelled") == 0 ? UMI_TEST_STATE_CANCELLED : UMI_TEST_STATE_NOT_RUN,
                   UMI_STATUS_CANCELLED);
        expected = UMI_TEST_ARCHIVE_INCONCLUSIVE;
        expected_count = 1;
    }
    else if (strcmp(name, "never-started") == 0)
    {
        completed = 0;
        expected = UMI_TEST_ARCHIVE_INCONCLUSIVE;
    }
    else if (strcmp(name, "partial") == 0)
    {
        completed = 3;
        expected = UMI_TEST_ARCHIVE_INCONCLUSIVE;
        expected_count = 1;
    }
    else if (strcmp(name, "root") == 0)
    {
        strcpy(origin.source_root, "/different/source");
        expected = UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
    }
    else if (strcmp(name, "repeat") == 0)
    {
        job->plan.repeat_count = 1;
        completed = 2;
        expected = UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
    }
    else if (strcmp(name, "stop-policy") == 0)
    {
        job->plan.stop_on_failure = true;
        expected = UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
    }
    else if (strcmp(name, "timeout-policy") == 0 || strcmp(name, "enabled") == 0)
    {
        if (strcmp(name, "enabled") == 0)
            job->requests[0].enabled = 0;
        else
            job->requests[0].timeout_ms = 999;
        expected = UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
        expected_count = 1;
    }
    else if (strcmp(name, "build-root") == 0 || strcmp(name, "configuration") == 0)
    {
        for (size_t i = 0; i < 2U; ++i)
            if (strcmp(name, "build-root") == 0)
                strcpy(job->requests[i].build_directory, "/other/build");
            else
                strcpy(job->requests[i].configuration, "Release");
        expected = UMI_TEST_ARCHIVE_ADDED;
    }
    else if (strcmp(name, "renamed") == 0 || strcmp(name, "duplicate") == 0)
    {
        strcpy(job->requests[0].name, strcmp(name, "duplicate") == 0 ? job->requests[1].name : "new test");
        strcpy(job->results[0].name, job->requests[0].name);
        strcpy(job->results[2].name, job->requests[0].name);
        if (strcmp(name, "duplicate") == 0)
            expected_status = UMI_STATUS_PARSE_ERROR;
        else
        {
            expected = UMI_TEST_ARCHIVE_ADDED;
            expected_count = 1;
        }
    }
    else if (strcmp(name, "reordered") == 0)
    {
        UmiCtestJobRequest temporary = job->requests[0];
        job->requests[0] = job->requests[1];
        job->requests[1] = temporary;
        /* Rediscovery may assign different IDs, while names and locations match. */
        strcpy(job->requests[0].test_id, "new.first");
        strcpy(job->requests[1].test_id, "new.second");
        for (size_t i = 0; i < 4U; ++i)
        {
            strcpy(job->results[i].test_id, job->requests[i % 2U].test_id);
            strcpy(job->results[i].name, job->requests[i % 2U].name);
        }
    }
    else if (strncmp(name, "identity-", 9) == 0)
    {
        if (strcmp(name, "identity-settings") == 0)
            origin.identity.configuration[0] = 'd';
        else if (strcmp(name, "identity-project") == 0)
            origin.identity.subject[0] = 'd';
        else if (strcmp(name, "identity-inputs") == 0)
            origin.identity.inputs[0] = 'd';
        else if (strcmp(name, "identity-legacy") == 0)
            memset(&origin.identity, 0, sizeof(origin.identity));
        else
            CHECK(strcmp(name, "identity-missing-inputs") == 0);
        if (strcmp(name, "identity-settings") == 0 || strcmp(name, "identity-project") == 0)
            expected = UMI_TEST_ARCHIVE_CONDITIONS_CHANGED;
    }
    else if (strcmp(name, "duration") == 0)
    {
        job->results[0].duration_ms = UINT64_MAX;
    }
    else if (strcmp(name, "revision") == 0)
    {
        origin.source_revision[0] = '\0';
    }
    else if (strcmp(name, "invalid") != 0 && strcmp(name, "missing") != 0 && strcmp(name, "cancel") != 0 &&
             strcmp(name, "transaction") != 0 && strcmp(name, "corrupt-summary") != 0 &&
             strcmp(name, "missing-chunk") != 0 && strcmp(name, "detached") != 0 && strcmp(name, "self") != 0)
    {
        CHECK(0);
    }
    Recount(job, completed);
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &after) == UMI_STATUS_OK);
    if (strcmp(name, "corrupt-summary") == 0)
    {
        after.run.passed = 3;
        after.run.failed = 1;
        char wire[4096];
        CHECK(UmiTestArchiveEncodeEntry(&after, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(umi_data_server_set(server, "ctest-archive/test/slot/1", wire) == UMI_STATUS_OK);
        expected_status = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(name, "missing-chunk") == 0)
    {
        CHECK(umi_data_server_delete(server, "ctest-archive/test/run/2/result/0/part/0") == UMI_STATUS_OK);
        expected_status = UMI_STATUS_PARSE_ERROR;
    }
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    uint64_t candidate = after.id;
    if (strcmp(name, "invalid") == 0)
    {
        candidate = 0;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "missing") == 0)
    {
        candidate = 91;
        expected_status = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "self") == 0)
        candidate = before.id;
    if (strcmp(name, "cancel") == 0)
    {
        umi_cancellation_token_request(token);
        expected_status = UMI_STATUS_CANCELLED;
    }
    if (strcmp(name, "transaction") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        expected_status = UMI_STATUS_BUSY;
    }
    UmiTestArchiveComparison *comparison = NULL;
    CHECK(UmiTestArchiveCompare(archive, before.id, candidate, token, &comparison) == expected_status);
    if (expected_status == UMI_STATUS_OK)
    {
        /* A failed read must preserve an already-owned comparison pointer. */
        UmiTestArchiveComparison *owned = comparison;
        CHECK(UmiTestArchiveCompare(archive, before.id, UINT64_MAX, NULL, &comparison) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(comparison == owned);
        UmiTestArchiveComparisonSummary summary = {0};
        CHECK(UmiTestArchiveComparisonRead(comparison, &summary) == UMI_STATUS_OK);
        CHECK(summary.counts[expected] == expected_count);
        CHECK(summary.selection_evidence_recorded);
        if (strncmp(name, "identity-", 9) == 0)
        {
            UmiJobIdentityComparison identity = strcmp(name, "identity-settings") == 0 ?
                UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION : strcmp(name, "identity-project") == 0 ?
                UMI_JOB_IDENTITY_DIFFERENT_SUBJECT : strcmp(name, "identity-inputs") == 0 ?
                UMI_JOB_IDENTITY_DIFFERENT_INPUTS : strcmp(name, "identity-legacy") == 0 ?
                UMI_JOB_IDENTITY_UNKNOWN : UMI_JOB_IDENTITY_INPUTS_UNRECORDED;
            CHECK(summary.identity_comparison == identity && summary.same_recorded_selection);
        }

        CHECK(summary.same_source_root == (strcmp(name, "root") != 0));
        CHECK(summary.same_recorded_revision == (strcmp(name, "revision") != 0));
        if (strcmp(name, "build-root") == 0 || strcmp(name, "configuration") == 0)
            CHECK(summary.row_count == 4 && summary.counts[UMI_TEST_ARCHIVE_REMOVED] == 2);
        if (strcmp(name, "detached") == 0)
        {
            CHECK(UmiTestArchiveRemove(archive, before.id) == UMI_STATUS_OK);
            UmiTestArchiveDestroy(archive);
            archive = NULL;
            umi_data_server_destroy(server);
            server = NULL;
        }
        UmiTestArchiveComparisonRow row = {0};
        CHECK(UmiTestArchiveComparisonRowAt(comparison, 0, &row) == UMI_STATUS_OK);
        UmiTestArchiveComparisonRow unchanged = row;
        CHECK(UmiTestArchiveComparisonRowAt(comparison, summary.row_count, &row) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&row, &unchanged, sizeof(row)) == 0);
        if (strcmp(name, "duration") == 0)
            CHECK(row.after.duration_ms == UINT64_MAX);
        if (strcmp(name, "never-started") == 0)
            CHECK(row.after.never_started == 2 && row.after.passed == 0);
        if (strcmp(name, "mixed") == 0)
            CHECK(row.after.failed == 1 && row.after.passed == 1);
    }
    else
        CHECK(comparison == NULL);
    if (strcmp(name, "transaction") == 0)
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    UmiTestArchiveComparisonDestroy(comparison);
    umi_cancellation_token_destroy(token);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}
