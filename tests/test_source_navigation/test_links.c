/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_navigation/test_links.c
 * PURPOSE: Verify provenance, retention, deduplication and ownership of source evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/source_links.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static void Result(UmiTestPlatformResultRegistry *r, unsigned sequence, const char *item,
    const char *session, const char *details, const char *message)
{
    UmiTestPlatformResultSnapshot s = {0};
    (void)snprintf(s.id, sizeof(s.id), "result-%u", sequence);
    strcpy(s.item_id, item); strcpy(s.session_id, session); strcpy(s.failure_details, details);
    strcpy(s.message, message); s.sequence = sequence; s.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    OK(umi_test_platform_result_registry_upsert(r, &s));
}
static void Output(UmiTestPlatformOutputRegistry *r, unsigned sequence, const char *text)
{
    UmiTestPlatformOutputSnapshot s = {0};
    (void)snprintf(s.id, sizeof(s.id), "output-%u", sequence);
    strcpy(s.item_id, "wanted"); strcpy(s.session_id, "run-one"); strcpy(s.stream, "stderr");
    strcpy(s.text, text); s.timestamp = sequence;
    OK(umi_test_platform_output_registry_upsert(r, &s));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiTestPlatformResultRegistry *results = NULL; UmiTestPlatformOutputRegistry *outputs = NULL;
    UmiTestEvidence *evidence = NULL; UmiTestSourceLinks *links = NULL;
    OK(umi_test_platform_result_registry_create(&results)); OK(umi_test_platform_output_registry_create(&outputs));
    if (strcmp(name, "selection") == 0 || strcmp(name, "ownership") == 0) {
        Result(results, 1, "wanted", "run-one", "src/first.c:2: assertion", "no source");
        Result(results, 2, "wanted-more", "run-one", "src/wrong.c:9: other", "");
        Output(outputs, 1, "205: src/second.c:5: output");
    } else if (strcmp(name, "duplicates") == 0) {
        Result(results, 1, "wanted", "run-one", "src/test.c:2: old", "");
        Result(results, 3, "wanted", "run-one", "src/test.c:2: new", "src/test.c:2: message");
        Output(outputs, 2, "src/test.c:2: repeated\nsrc/test.c:2:3: another column");
    } else if (strcmp(name, "sessions") == 0) {
        Result(results, 1, "wanted", "run-one", "src/test.c:2: first run", "");
        Result(results, 2, "wanted", "run-two", "src/test.c:2: second run", "");
    } else if (strcmp(name, "fragments") == 0) {
        Output(outputs, 1, "src/test.c:"); Output(outputs, 2, "70: assertion");
        Output(outputs, 3, "line 28: assertion\nunknown output");
    } else if (strcmp(name, "capacity") == 0) {
        for (unsigned i=1; i<=30; ++i) {
            char lines[1000] = ""; size_t used=0;
            for (unsigned j=1; j<=10; ++j) {
                int n=snprintf(lines+used, sizeof(lines)-used, "src/test.c:%u: assertion\n", i*10+j);
                CHECK(n>0 && (size_t)n<sizeof(lines)-used); used+=(size_t)n;
            }
            Output(outputs, i, lines);
        }
    } else if (strcmp(name, "retention") == 0) {
        for (unsigned i=1; i<=40; ++i) {
            char line[100]; (void)snprintf(line, sizeof(line), "test.c:%u: check", i);
            Result(results, i, "wanted", "run-one", line, "");
        }
    } else CHECK(strcmp(name, "empty") == 0);
    OK(UmiTestEvidenceCreate(results, outputs, "wanted", NULL, &evidence));
    OK(UmiTestSourceLinksCreate(evidence, &links));
    UmiTestEvidenceDestroy(evidence); umi_test_platform_result_registry_destroy(results);
    umi_test_platform_output_registry_destroy(outputs);
    UmiTestSourceLinksSummary summary; OK(UmiTestSourceLinksGetSummary(links, &summary));
    UmiTestSourceLink first;
    if (summary.retained_links != 0) OK(UmiTestSourceLinksAt(links, 0, &first));
    if (strcmp(name, "selection") == 0 || strcmp(name, "ownership") == 0) {
        CHECK(summary.retained_links == 2 && summary.evidence.matching_results == 1);
        CHECK(strcmp(first.item_id, "wanted") == 0 && strcmp(first.location.path, "src/first.c") == 0);
        CHECK(first.origin == UMI_TEST_SOURCE_RESULT_DETAILS && summary.unparsed_lines == 1);
        UmiTestSourceLink second; OK(UmiTestSourceLinksAt(links, 1, &second));
        CHECK(second.origin == UMI_TEST_SOURCE_OUTPUT && strcmp(second.location.path, "src/second.c") == 0);
    } else if (strcmp(name, "duplicates") == 0) {
        CHECK(summary.retained_links == 2 && summary.duplicate_lines == 3 && summary.recognised_lines == 5);
        CHECK(strcmp(first.record_id, "result-3") == 0 && strcmp(first.location.message, "new") == 0);
    } else if (strcmp(name, "sessions") == 0) {
        CHECK(summary.retained_links == 2 && summary.duplicate_lines == 0 && strcmp(first.session_id, "run-two") == 0);
    } else if (strcmp(name, "fragments") == 0) CHECK(summary.retained_links == 0 && summary.unparsed_lines == 4);
    else if (strcmp(name, "capacity") == 0) CHECK(summary.retained_links == 256 && summary.capacity_dropped == 44 && summary.recognised_lines == 300);
    else if (strcmp(name, "retention") == 0) CHECK(summary.retained_links == 32 && summary.evidence.matching_results == 40 && first.location.line == 40);
    else CHECK(summary.retained_links == 0 && summary.evidence.matching_results == 0);
    UmiTestSourceLink unchanged, before; memset(&unchanged, 0x5a, sizeof(unchanged)); memcpy(&before, &unchanged, sizeof(before));
    CHECK(UmiTestSourceLinksAt(links, summary.retained_links, &unchanged) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&unchanged, &before, sizeof(before)) == 0);
    UmiTestSourceLinksDestroy(links); links = (UmiTestSourceLinks *)(void *)&first;
    CHECK(UmiTestSourceLinksCreate(NULL, &links) == UMI_STATUS_INVALID_ARGUMENT && links == NULL);
    return 0;
}
