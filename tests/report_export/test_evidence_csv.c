/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_evidence_csv.c
 * PURPOSE: Verify selected-test export identity, retention disclosure and copied diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_platform/evidence_csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static void Add(UmiTestPlatformResultRegistry *results, UmiTestPlatformOutputRegistry *outputs,
    unsigned index, const char *item, const char *message)
{
    UmiTestPlatformResultSnapshot r = {0};
    (void)snprintf(r.id, sizeof(r.id), "result-%u", index);
    strcpy(r.item_id, item); strcpy(r.session_id, "run-one"); strcpy(r.message, message);
    strcpy(r.failure_details, "first line\n\"second\",line");
    r.sequence = index; r.outcome = UMI_TEST_PLATFORM_OUTCOME_SKIPPED; r.duration_ms = 1.25;
    OK(umi_test_platform_result_registry_upsert(results, &r));
    UmiTestPlatformOutputSnapshot o = {0};
    (void)snprintf(o.id, sizeof(o.id), "output-%u", index);
    strcpy(o.item_id, item); strcpy(o.session_id, "run-one"); strcpy(o.stream, "stderr");
    strcpy(o.text, "caf\xc3\xa9\nretained output"); o.timestamp = index;
    OK(umi_test_platform_output_registry_upsert(outputs, &o));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); UmiTestPlatformResultRegistry *results = NULL;
    UmiTestPlatformOutputRegistry *outputs = NULL; UmiTestEvidence *capture = NULL; UmiCsvDocument *doc = NULL;
    OK(umi_test_platform_result_registry_create(&results)); OK(umi_test_platform_output_registry_create(&outputs));
    Add(results, outputs, 1, "wanted", "=not a formula to execute");
    Add(results, outputs, 2, "other", "unrelated evidence");
    const char *item = "wanted";
    if (strcmp(argv[1], "empty") == 0) item = "missing";
    if (strcmp(argv[1], "retention") == 0) {
        for (unsigned i = 3; i < 103; ++i) Add(results, outputs, i, "wanted", "retained evidence");
    }
    if (strcmp(argv[1], "invalid-text") == 0) Add(results, outputs, 3, "wanted", "\xed\xa0\x80");
    OK(UmiTestEvidenceCreate(results, outputs, item, NULL, &capture));
    UmiTestEvidenceSummary summary; OK(UmiTestEvidenceGetSummary(capture, &summary));
    umi_test_platform_result_registry_destroy(results); umi_test_platform_output_registry_destroy(outputs);
    if (strcmp(argv[1], "invalid-text") == 0) {
        CHECK(UmiTestEvidenceExportCsv(capture, &doc) == UMI_STATUS_INVALID_ARGUMENT && doc == NULL);
    } else {
        OK(UmiTestEvidenceExportCsv(capture, &doc));
        CHECK(UmiCsvDocumentRows(doc) == 2U + summary.retained_results + summary.retained_output);
        const char *text = UmiCsvDocumentData(doc);
        CHECK(strstr(text, "unrelated evidence") == NULL && strstr(text, "evidence-summary") != NULL);
        if (strcmp(argv[1], "empty") == 0) CHECK(UmiCsvDocumentRows(doc) == 2);
        else if (strcmp(argv[1], "retention") == 0) {
            CHECK(summary.matching_results == 101 && summary.retained_results == 32);
            CHECK(summary.matching_output == 101 && summary.retained_output == 64);
            CHECK(strstr(text, "\"101\",\"32\",\"101\",\"64\"") != NULL);
            CHECK(strstr(text, "result-1\"") == NULL && strstr(text, "output-1\"") == NULL);
        } else {
            CHECK(strcmp(argv[1], "selection") == 0 || strcmp(argv[1], "ownership") == 0);
            CHECK(strstr(text, "\"'=not a formula to execute\"") != NULL);
            CHECK(strstr(text, "\"\"second\"\",line") != NULL);
            CHECK(strstr(text, "caf\xc3\xa9\nretained output") != NULL);
        }
    }
    UmiTestEvidenceDestroy(capture);
    if (strcmp(argv[1], "ownership") == 0) CHECK(strstr(UmiCsvDocumentData(doc), "run-one") != NULL);
    UmiCsvDocumentDestroy(doc); return 0;
}
