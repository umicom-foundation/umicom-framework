/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_selected_test_evidence.c
 * PURPOSE: Verify exact identity, ordering, limits and lifetime of captured test evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/evidence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

static void Add(UmiTestPlatformResultRegistry *results, UmiTestPlatformOutputRegistry *output,
    const char *id, const char *item, const char *session, uint64_t order)
{
    UmiTestPlatformResultSnapshot r = {0};
    strcpy(r.id, id);
    strcpy(r.item_id, item);
    strcpy(r.session_id, session);
    strcpy(r.message, "retained result");
    r.sequence = order;
    r.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    r.exit_code = 1;
    CHECK(umi_test_platform_result_registry_upsert(results, &r) == UMI_STATUS_OK);
    UmiTestPlatformOutputSnapshot o = {0};
    strcpy(o.id, id);
    strcpy(o.item_id, item);
    strcpy(o.session_id, session);
    strcpy(o.stream, "stderr");
    strcpy(o.text, "owned output");
    o.timestamp = order;
    CHECK(umi_test_platform_output_registry_upsert(output, &o) == UMI_STATUS_OK);
}

int main(void)
{
    UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformOutputRegistry *output = NULL;
    UmiTestEvidence *capture = NULL;
    UmiTestEvidenceSummary summary;
    UmiTestPlatformResultSnapshot r;
    UmiTestPlatformOutputSnapshot o;
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    CHECK(umi_test_platform_output_registry_create(&output) == UMI_STATUS_OK);
    Add(results, output, "newest", "test.a", "run.2", 90U);
    Add(results, output, "oldest", "test.a", "run.1", 10U);
    Add(results, output, "other", "test.ab", "run.2", 900U);
    Add(results, output, "same-time", "test.a", "run.2", 90U);
    CHECK(UmiTestEvidenceCreate(results, output, "test.a", NULL, &capture) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceGetSummary(capture, &summary) == UMI_STATUS_OK);
    CHECK(summary.matching_results == 3U && summary.matching_output == 3U);
    CHECK(UmiTestEvidenceResultAt(capture, 0U, &r) == UMI_STATUS_OK && strcmp(r.id, "same-time") == 0);
    CHECK(r.outcome == UMI_TEST_PLATFORM_OUTCOME_FAILED && r.exit_code == 1);
    CHECK(UmiTestEvidenceOutputAt(capture, 2U, &o) == UMI_STATUS_OK && strcmp(o.id, "oldest") == 0);
    UmiTestPlatformOutputSnapshot before = o;
    CHECK(UmiTestEvidenceOutputAt(capture, 3U, &o) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&before, &o, sizeof(o)) == 0);
    UmiTestEvidenceDestroy(capture);
    CHECK(UmiTestEvidenceCreate(results, output, "test.a", "run.1", &capture) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceGetSummary(capture, &summary) == UMI_STATUS_OK && summary.retained_results == 1U);
    CHECK(UmiTestEvidenceResultAt(capture, 0U, &r) == UMI_STATUS_OK && strcmp(r.id, "oldest") == 0);
    UmiTestEvidenceDestroy(capture);

    for (unsigned i = 0U; i < 80U; ++i) {
        char id[32];
        (void)snprintf(id, sizeof(id), "selected-%u", i);
        Add(results, output, id, "bounded", "run", i);
    }
    /* Unrelated newer records must not consume this test's retention limit. */
    for (unsigned i = 0U; i < 80U; ++i) {
        char id[32];
        (void)snprintf(id, sizeof(id), "unrelated-%u", i);
        Add(results, output, id, "other-test", "run", 1000U + i);
    }
    CHECK(UmiTestEvidenceCreate(results, output, "bounded", NULL, &capture) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceGetSummary(capture, &summary) == UMI_STATUS_OK);
    CHECK(summary.matching_results == 80U && summary.retained_results == UMI_TEST_EVIDENCE_RESULT_LIMIT);
    CHECK(summary.matching_output == 80U && summary.retained_output == UMI_TEST_EVIDENCE_OUTPUT_LIMIT);
    CHECK(UmiTestEvidenceResultAt(capture, 0U, &r) == UMI_STATUS_OK && r.sequence == 79U);
    CHECK(UmiTestEvidenceResultAt(capture, 31U, &r) == UMI_STATUS_OK && r.sequence == 48U);
    CHECK(UmiTestEvidenceOutputAt(capture, 63U, &o) == UMI_STATUS_OK && o.timestamp == 16U);
    umi_test_platform_result_registry_clear(results);
    umi_test_platform_output_registry_clear(output);
    CHECK(UmiTestEvidenceResultAt(capture, 0U, &r) == UMI_STATUS_OK && r.sequence == 79U);
    UmiTestEvidenceDestroy(capture);
    CHECK(UmiTestEvidenceCreate(results, output, "absent", NULL, &capture) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceGetSummary(capture, &summary) == UMI_STATUS_OK && summary.matching_results == 0U && summary.retained_output == 0U);
    UmiTestEvidenceDestroy(capture);
    capture = NULL;
    char invalid[128];
    memset(invalid, 'x', sizeof(invalid));
    CHECK(UmiTestEvidenceCreate(results, output, invalid, NULL, &capture) == UMI_STATUS_INVALID_ARGUMENT && capture == NULL);
    CHECK(UmiTestEvidenceCreate(results, output, "", NULL, &capture) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTestEvidenceCreate(results, output, "test", invalid, &capture) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTestEvidenceCreate(NULL, output, "test", NULL, &capture) == UMI_STATUS_INVALID_ARGUMENT);
    Add(results, output, "owned", "test", "run", 1U);
    CHECK(UmiTestEvidenceCreate(results, output, "test", NULL, &capture) == UMI_STATUS_OK);
    umi_test_platform_result_registry_destroy(results);
    umi_test_platform_output_registry_destroy(output);
    CHECK(UmiTestEvidenceOutputAt(capture, 0U, &o) == UMI_STATUS_OK && strcmp(o.text, "owned output") == 0);
    UmiTestEvidenceDestroy(capture);
    return 0;
}
