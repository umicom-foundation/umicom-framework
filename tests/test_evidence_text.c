/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_evidence_text.c
 * PURPOSE: Verify exact selected-test reporting, outcomes and owned text lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/evidence_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static void Add(UmiTestPlatformResultRegistry *results, UmiTestPlatformOutputRegistry *output,
    const char *id, const char *item, const char *session, unsigned sequence, unsigned outcome)
{
    UmiTestPlatformResultSnapshot result = {0};
    snprintf(result.id, sizeof result.id, "%s", id);
    snprintf(result.item_id, sizeof result.item_id, "%s", item);
    snprintf(result.session_id, sizeof result.session_id, "%s", session);
    snprintf(result.message, sizeof result.message, "message %s", id);
    snprintf(result.failure_details, sizeof result.failure_details, "details %s", id);
    result.sequence = sequence; result.outcome = outcome; result.duration_ms = 12.5;
    result.exit_code = outcome == UMI_TEST_PLATFORM_OUTCOME_PASSED ? 0 : 7;
    result.flaky = sequence == 7U;
    CHECK(umi_test_platform_result_registry_upsert(results, &result) == UMI_STATUS_OK);
    UmiTestPlatformOutputSnapshot record = {0};
    snprintf(record.id, sizeof record.id, "%s", id);
    snprintf(record.item_id, sizeof record.item_id, "%s", item);
    snprintf(record.session_id, sizeof record.session_id, "%s", session);
    snprintf(record.stream, sizeof record.stream, "%s", "stderr");
    snprintf(record.text, sizeof record.text, "output %s\nnext line", id); record.timestamp = sequence;
    CHECK(umi_test_platform_output_registry_upsert(output, &record) == UMI_STATUS_OK);
}
static char *Report(UmiTestPlatformResultRegistry *results, UmiTestPlatformOutputRegistry *output,
    const char *item, const char *session)
{
    UmiTestEvidence *evidence = NULL;
    char *text = NULL; size_t bytes = 0U;
    CHECK(UmiTestEvidenceCreate(results, output, item, session, &evidence) == UMI_STATUS_OK);
    CHECK(UmiTestEvidenceFormatText(evidence, &text, &bytes) == UMI_STATUS_OK);
    CHECK(text != NULL && strlen(text) == bytes && bytes <= UMI_TEST_EVIDENCE_TEXT_MAX_BYTES);
    UmiTestEvidenceDestroy(evidence); return text;
}
int main(void)
{
    UmiTestPlatformResultRegistry *results = NULL; UmiTestPlatformOutputRegistry *output = NULL;
    CHECK(umi_test_platform_result_registry_create(&results) == UMI_STATUS_OK);
    CHECK(umi_test_platform_output_registry_create(&output) == UMI_STATUS_OK);
    char *text = Report(results, output, "sample", NULL);
    CHECK(strstr(text, "Selected test: sample\n") != NULL);
    CHECK(strstr(text, "Retained results: 0 of 0 | output records: 0 of 0") != NULL);
    CHECK(strstr(text, "No result is retained for this test. This is not a pass.") != NULL);
    CHECK(strstr(text, "No output retained for this test.") != NULL);
    UmiTestEvidenceTextFree(text);
    static const char *const outcomes[] = {"not-run", "passed", "failed", "skipped", "cancelled", "timed-out"};
    for (unsigned outcome = 0U; outcome < sizeof outcomes / sizeof outcomes[0]; ++outcome) {
        umi_test_platform_result_registry_clear(results); umi_test_platform_output_registry_clear(output);
        Add(results, output, "selected", "sample", "run", 7U, outcome);
        Add(results, output, "unrelated", "sample.more", "run", 9U, UMI_TEST_PLATFORM_OUTCOME_PASSED);
        text = Report(results, output, "sample", NULL);
        char label[80]; snprintf(label, sizeof label, "[run] %s |", outcomes[outcome]);
        CHECK(strstr(text, label) != NULL && strstr(text, "unrelated") == NULL);
        CHECK(strstr(text, " | flaky\nmessage selected\ndetails selected") != NULL);
        CHECK(strstr(text, "[run / stderr] output selected\nnext line") != NULL);
        UmiTestEvidenceTextFree(text);
    }
    umi_test_platform_result_registry_clear(results); umi_test_platform_output_registry_clear(output);
    Add(results, output, "old", "sample", "run.1", 1U, UMI_TEST_PLATFORM_OUTCOME_FAILED);
    Add(results, output, "new", "sample", "run.2", 2U, UMI_TEST_PLATFORM_OUTCOME_SKIPPED);
    text = Report(results, output, "sample", "run.1");
    CHECK(strstr(text, "Session filter: run.1") != NULL && strstr(text, "message new") == NULL);
    UmiTestEvidenceTextFree(text);
    text = Report(results, output, "sample", NULL);
    const char *newer = strstr(text, "message new"), *older = strstr(text, "message old");
    CHECK(newer != NULL && older != NULL && newer < older);
    UmiTestEvidenceTextFree(text);
    for (unsigned i = 0U; i < 80U; ++i) {
        char id[32]; snprintf(id, sizeof id, "bounded-%u", i);
        Add(results, output, id, "bounded", "", i, UMI_TEST_PLATFORM_OUTCOME_FAILED);
    }
    text = Report(results, output, "bounded", NULL);
    CHECK(strstr(text, "Retained results: 32 of 80 | output records: 64 of 80") != NULL);
    CHECK(strstr(text, "session unspecified") != NULL);
    UmiTestEvidenceTextFree(text);
    /* Maximum retained payloads exercise the measured allocation rather than
     * relying on a fixed presentation buffer that could truncate diagnostics. */
    UmiTestPlatformOutputSnapshot full = {0};
    strcpy(full.item_id, "maximum"); strcpy(full.stream, "stderr");
    memset(full.text, 'x', sizeof full.text - 1U);
    for (unsigned i = 0U; i < UMI_TEST_EVIDENCE_OUTPUT_LIMIT; ++i) {
        snprintf(full.id, sizeof full.id, "maximum-%u", i); full.timestamp = i;
        CHECK(umi_test_platform_output_registry_upsert(output, &full) == UMI_STATUS_OK);
    }
    text = Report(results, output, "maximum", NULL);
    CHECK(strlen(text) > UMI_TEST_EVIDENCE_OUTPUT_LIMIT * (sizeof full.text - 1U));
    CHECK(strstr(text, full.text) != NULL && strstr(text, "output records: 64 of 64") != NULL);
    UmiTestEvidenceTextFree(text);
    UmiTestPlatformOutputSnapshot raw = {0};
    strcpy(raw.id, "raw"); strcpy(raw.item_id, "raw-test"); strcpy(raw.stream, "stderr");
    raw.text[0] = (char)0xff; raw.text[1] = 'x';
    CHECK(umi_test_platform_output_registry_upsert(output, &raw) == UMI_STATUS_OK);
    UmiTestEvidence *owned = NULL;
    CHECK(UmiTestEvidenceCreate(results, output, "raw-test", NULL, &owned) == UMI_STATUS_OK);
    umi_test_platform_result_registry_destroy(results); umi_test_platform_output_registry_destroy(output);
    CHECK(UmiTestEvidenceFormatText(owned, &text, NULL) == UMI_STATUS_OK);
    CHECK(strstr(text, raw.text) != NULL && strstr(text, "This is not a pass") != NULL);
    UmiTestEvidenceDestroy(owned);
    CHECK(strstr(text, raw.text) != NULL); UmiTestEvidenceTextFree(text);
    char sentinel = 0; text = &sentinel; size_t unchanged = 17U;
    CHECK(UmiTestEvidenceFormatText(NULL, &text, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(text == NULL && unchanged == 17U);
    CHECK(UmiTestEvidenceFormatText(NULL, NULL, &unchanged) == UMI_STATUS_INVALID_ARGUMENT && unchanged == 17U);
    UmiTestEvidenceTextFree(NULL);
    return 0;
}
