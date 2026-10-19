/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_report.c
 * Purpose: Verify complete escaped evidence and all-or-nothing output sizing.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, strcmp(name, "many-rows") == 0 ? 5000U : 1U, false) == 0);
    if (strcmp(name, "passed") == 0 || strcmp(name, "many-rows") == 0) {
        REQUIRE(QualificationRecordAll(&f) == 0);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    } else if (strcmp(name, "skipped") == 0) {
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &f.identity, &result) == UMI_STATUS_OK);
        result = QualificationResult(UMI_QUALIFICATION_SKIPPED);
        REQUIRE(UmiReleaseQualificationRecordTest(f.run, &f.identity, "test.000000", &result) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    } else if (strcmp(name, "framing") == 0) {
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        UmiReleaseInventoryDestroy(f.inventory); f.inventory = NULL;
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        REQUIRE(FixtureTest(&f.text, "odd\tname\nend", "library", NULL, false));
        REQUIRE(FixtureParse(&f.text, &f.inventory) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, true, &f.plan) == UMI_STATUS_OK);
        UmiReleaseSourceOptions options = {.read = QualificationRead,
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &f.before) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain, &f.run) == UMI_STATUS_OK);
    } else REQUIRE(strcmp(name, "missing") == 0 || strcmp(name, "capacity") == 0 ||
                   strcmp(name, "invalid") == 0 || strcmp(name, "deterministic") == 0);
    size_t required = 0U;
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, NULL, 0U, &required) == UMI_STATUS_OK);
    REQUIRE(required > 1U);
    char *output = malloc(required), *copy = malloc(required);
    REQUIRE(output != NULL && copy != NULL);
    memset(output, 'x', required);
    if (strcmp(name, "capacity") == 0 || strcmp(name, "invalid") == 0) {
        size_t sentinel = 123U;
        UmiStatus status = strcmp(name, "capacity") == 0 ?
            UmiReleaseQualificationFormatReport(f.run, output, required - 1U, &sentinel) :
            UmiReleaseQualificationFormatReport(NULL, output, required, &sentinel);
        REQUIRE(status == (strcmp(name, "capacity") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_ARGUMENT));
        REQUIRE(sentinel == 123U);
        for (size_t i = 0U; i < required; ++i) REQUIRE(output[i] == 'x');
    }
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, output, required, &required) == UMI_STATUS_OK);
    REQUIRE(strlen(output) + 1U == required);
    REQUIRE(strncmp(output, "UMICOM-QUALIFICATION\t1\n", 23U) == 0);
    REQUIRE(strstr(output, "\nidentity\t") != NULL);
    REQUIRE(strstr(output, f.identity.execution) != NULL);
    REQUIRE(strcmp(output + required - 5U, "end\n") == 0);
    if (strcmp(name, "missing") == 0) {
        REQUIRE(strstr(output, "\nbuild\tmissing\n") != NULL);
        REQUIRE(strstr(output, "\nsource-after\t\n") != NULL);
    }
    if (strcmp(name, "skipped") == 0) REQUIRE(strstr(output, "\tskipped\t") != NULL);
    if (strcmp(name, "framing") == 0) {
        REQUIRE(strstr(output, "6f6464096e616d650a656e64") != NULL);
        REQUIRE(strstr(output, "odd\tname\nend") == NULL);
    }
    if (strcmp(name, "many-rows") == 0) {
        size_t count = 0U;
        for (const char *row = output; (row = strstr(row, "\ntest\t")) != NULL; row += 6U) ++count;
        REQUIRE(count == 5000U);
    }
    REQUIRE(UmiReleaseQualificationFormatReport(f.run, copy, required, &required) == UMI_STATUS_OK);
    REQUIRE(strcmp(output, copy) == 0);
    REQUIRE(strcmp(UmiReleaseQualificationOutcomeText((UmiReleaseQualificationOutcome)99), "unknown") == 0);
    REQUIRE(strcmp(UmiReleaseQualificationBlockerText((UmiReleaseQualificationBlocker)0), "Unknown") == 0);
    free(copy); free(output);
    QualificationDestroy(&f);
    return 0;
}
