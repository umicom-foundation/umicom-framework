/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_evidence_runs.c
 * PURPOSE: Guard exact-run evidence, independent ownership and stale selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/evidence_runs.h"
#include "umicom/test_platform/source_links.h"
#include "umicom/test_platform/evidence_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

static void Result(UmiTestPlatformResultRegistry *r, const char *id,
    const char *item, const char *run, int outcome, uint64_t sequence)
{
    UmiTestPlatformResultSnapshot row = {0};
    strcpy(row.id, id); strcpy(row.item_id, item); strcpy(row.session_id, run);
    strcpy(row.failure_details, "source.c:7:3: error: retained failure");
    row.outcome = outcome; row.sequence = sequence;
    CHECK(umi_test_platform_result_registry_upsert(r, &row) == UMI_STATUS_OK);
}
static void Output(UmiTestPlatformOutputRegistry *o, const char *id,
    const char *item, const char *run)
{
    UmiTestPlatformOutputSnapshot row = {0};
    strcpy(row.id, id); strcpy(row.item_id, item); strcpy(row.session_id, run);
    strcpy(row.text, "other.c:9:2: error: retained output");
    CHECK(umi_test_platform_output_registry_upsert(o, &row) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiTestPlatformResultRegistry *r = NULL, *other = NULL;
    UmiTestPlatformOutputRegistry *o = NULL;
    UmiTestEvidenceRuns *runs = NULL;
    UmiTestEvidenceRunsSummary summary;
    UmiTestEvidenceRun row;
    UmiTestEvidence *e = NULL;
    UmiTestEvidenceSummary evidence;
    CHECK(umi_test_platform_result_registry_create(&r) == UMI_STATUS_OK);
    CHECK(umi_test_platform_output_registry_create(&o) == UMI_STATUS_OK);
    if (strcmp(name, "empty") != 0 && strcmp(name, "arguments") != 0) {
        Result(r, "old", "test.a", "run.z", UMI_TEST_PLATFORM_OUTCOME_FAILED, 1U);
        Result(r, "new", "test.a", "run.a", UMI_TEST_PLATFORM_OUTCOME_PASSED, 90U);
        Result(r, "ignored", "test.ab", "unrelated", UMI_TEST_PLATFORM_OUTCOME_PASSED, 100U);
        Output(o, "output", "test.a", "run.z");
        Output(o, "output-only", "test.a", "run.b");
        Result(r, "unknown", "test.a", "", UMI_TEST_PLATFORM_OUTCOME_NOT_RUN, 0U);
        Output(o, "unknown-output", "test.a", "");
    }
    CHECK(UmiTestEvidenceRunsCreate(r, o, "test.a", &runs) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceRunsRead(runs, &summary) == UMI_STATUS_OK);
    if (strcmp(name, "empty") == 0) {
        CHECK(summary.runs == 0U && summary.matching_results == 0U);
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, NULL, &e) == UMI_STATUS_OK);
    } else if (strcmp(name, "catalogue") == 0) {
        CHECK(summary.runs == 3U && summary.matching_results == 3U && summary.matching_output == 3U);
        CHECK(summary.unspecified_results == 1U && summary.unspecified_output == 1U);
        CHECK(UmiTestEvidenceRunsAt(runs, 0U, &row) == UMI_STATUS_OK && strcmp(row.session_id, "run.a") == 0);
        CHECK(row.results == 1U && row.output == 0U);
        CHECK(UmiTestEvidenceRunsAt(runs, 2U, &row) == UMI_STATUS_OK && strcmp(row.session_id, "run.z") == 0);
        CHECK(row.results == 1U && row.output == 1U);
    } else if (strcmp(name, "all") == 0) {
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "", &e) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceGetSummary(e, &evidence) == UMI_STATUS_OK);
        CHECK(evidence.matching_results == 3U && evidence.matching_output == 3U);
    } else if (strcmp(name, "exact") == 0 || strcmp(name, "source-links") == 0) {
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "run.z", &e) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceGetSummary(e, &evidence) == UMI_STATUS_OK);
        CHECK(evidence.matching_results == 1U && evidence.matching_output == 1U);
        UmiTestPlatformResultSnapshot chosen;
        CHECK(UmiTestEvidenceResultAt(e, 0U, &chosen) == UMI_STATUS_OK && chosen.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED);
        if (strcmp(name, "source-links") == 0) {
        UmiTestSourceLinks *links = NULL;
        CHECK(UmiTestSourceLinksCreate(e, &links) == UMI_STATUS_OK);
        UmiTestSourceLinksSummary s;
        CHECK(UmiTestSourceLinksGetSummary(links, &s) == UMI_STATUS_OK && s.retained_links == 2U);
        for (size_t i = 0U; i < s.retained_links; ++i) {
            UmiTestSourceLink link;
            CHECK(UmiTestSourceLinksAt(links, i, &link) == UMI_STATUS_OK);
            CHECK(strcmp(link.session_id, "run.z") == 0);
        }
        UmiTestSourceLinksDestroy(links);
        }
    } else if (strcmp(name, "output-only") == 0) {
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "run.b", &e) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceGetSummary(e, &evidence) == UMI_STATUS_OK);
        CHECK(evidence.matching_results == 0U && evidence.matching_output == 1U);
    } else if (strcmp(name, "unknown-run") == 0) {
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "run", &e) == UMI_STATUS_NOT_FOUND && e == NULL);
    } else if (strcmp(name, "stale-result") == 0) {
        Result(r, "later", "test.a", "run.z", UMI_TEST_PLATFORM_OUTCOME_SKIPPED, 99U);
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "run.z", &e) == UMI_STATUS_INVALID_STATE && e == NULL);
    } else if (strcmp(name, "stale-output") == 0) {
        Output(o, "later", "test.a", "run.z");
        CHECK(UmiTestEvidenceRunsValidate(runs, r, o) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "identity") == 0) {
        CHECK(umi_test_platform_result_registry_create(&other) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceRunsValidate(runs, other, o) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "lifetime") == 0) {
        umi_test_platform_result_registry_destroy(r); r = NULL;
        umi_test_platform_output_registry_destroy(o); o = NULL;
        CHECK(UmiTestEvidenceRunsAt(runs, 2U, &row) == UMI_STATUS_OK && row.results == 1U);
    } else if (strcmp(name, "older-run") == 0) {
        UmiTestEvidenceRunsDestroy(runs); runs = NULL;
        for (unsigned i = 0U; i < 100U; ++i) {
            char id[32]; (void)snprintf(id, sizeof id, "new-%u", i);
            Result(r, id, "test.a", "run.a", UMI_TEST_PLATFORM_OUTCOME_PASSED, i + 100U);
        }
        CHECK(UmiTestEvidenceRunsCreate(r, o, "test.a", &runs) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "run.z", &e) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceGetSummary(e, &evidence) == UMI_STATUS_OK && evidence.retained_results == 1U);
    } else if (strcmp(name, "outcomes") == 0) {
        UmiTestEvidenceRunsDestroy(runs); runs = NULL;
        for (int i = 0; i <= 5; ++i) {
            char id[32]; (void)snprintf(id, sizeof id, "state-%d", i);
            Result(r, id, "test.a", "states", i, (uint64_t)i);
        }
        CHECK(UmiTestEvidenceRunsCreate(r, o, "test.a", &runs) == UMI_STATUS_OK);
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, "states", &e) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 6U; ++i) {
            UmiTestPlatformResultSnapshot result;
            CHECK(UmiTestEvidenceResultAt(e, i, &result) == UMI_STATUS_OK);
            CHECK(result.outcome == 5 - (int)i);
        }
    } else if (strcmp(name, "arguments") == 0) {
        char invalid[128]; memset(invalid, 'x', sizeof invalid);
        UmiTestEvidenceRun sentinel; memset(&sentinel, 0x55, sizeof sentinel); row = sentinel;
        CHECK(UmiTestEvidenceRunsAt(runs, 0U, &row) == UMI_STATUS_NOT_FOUND && memcmp(&row, &sentinel, sizeof row) == 0);
        CHECK(UmiTestEvidenceRunsCapture(runs, r, o, invalid, &e) == UMI_STATUS_INVALID_ARGUMENT && e == NULL);
        UmiTestEvidenceRuns *bad = NULL;
        CHECK(UmiTestEvidenceRunsCreate(r, o, invalid, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiTestEvidenceRunsCreate(r, o, "", &bad) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestEvidenceRunsCreate(NULL, o, "test.a", &bad) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestEvidenceRunsCreate(r, o, "test.a", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestEvidenceRunsRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestEvidenceRunsValidate(NULL, r, o) == UMI_STATUS_INVALID_ARGUMENT);
    } else { CHECK(!"unknown case"); }
    UmiTestEvidenceDestroy(e);
    UmiTestEvidenceRunsDestroy(runs);
    umi_test_platform_result_registry_destroy(other);
    umi_test_platform_result_registry_destroy(r);
    umi_test_platform_output_registry_destroy(o);
    return 0;
}
