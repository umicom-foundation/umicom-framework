/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_suite.c
 *
 * PURPOSE:
 *   Check ordered CTest suite results, cancellation and catalogue ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, failures;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    ++failures; fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
} } while (0)

/* Add copied catalogue records. No caller pointer becomes suite-owned. */
static void Add(UmiTestSuite *suite, const char *id, const char *name,
    int enabled, uint32_t timeout)
{
    UmiTestCase item;
    umi_test_case_init(&item, id, name);
    item.enabled = enabled; item.timeout_ms = timeout;
    CHECK(umi_test_suite_add(suite, &item) == UMI_STATUS_OK);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *directory = argv[1];
    UmiTestSuite *suite = NULL;
    UmiTestResult *results = calloc(8U, sizeof *results);
    if (results == NULL) return 2;
    UmiTestRunSummary summary;
    UmiCtestRunOptions options = {0}; options.configuration = "Debug";
    CHECK(umi_test_suite_create("notes", "Umicom Notes", &suite) == UMI_STATUS_OK);
    if (suite == NULL) { free(results); return 2; }
    CHECK(UmiCtestRunSuite(directory, suite, &options, results, 8U, &summary) == UMI_STATUS_NOT_FOUND);
    CHECK(summary.total == 0U && summary.passed == 0U);
    Add(suite, "notes.id.literal", "notes.save[1]+", 1, 0U);
    Add(suite, "notes.id.skip", "notes.skip", 1, 0U);
    Add(suite, "notes.id.failure", "notes.fail", 1, 0U);
    Add(suite, "notes.id.missing", "notes.not-in-this-catalogue", 1, 0U);
    /* A disabled record names a sentinel test that fails if it ever executes. */
    Add(suite, "notes.id.disabled", "notesXsave", 0, 0U);
    Add(suite, "notes.id.timeout", "notes.process-timeout", 1, 50U);
    Add(suite, "notes.id.configuration", "notes.configuration", 1, 0U);
    memset(results, 0x5A, 8U * sizeof *results);
    unsigned char before[sizeof(UmiTestResult)]; memcpy(before, results, sizeof before);
    CHECK(UmiCtestRunSuite(directory, suite, &options, results, 6U, &summary) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, results, sizeof before) == 0 && summary.total == 0U);
    CHECK(UmiCtestRunSuite(directory, suite, &options, results, 8U, &summary) == UMI_STATUS_INTERNAL_ERROR);
    CHECK(summary.total == 7U && summary.passed == 2U && summary.skipped == 2U);
    CHECK(summary.failed == 2U && summary.timed_out == 1U && summary.cancelled == 0U);
    CHECK(strcmp(results[0].test_id, "notes.id.literal") == 0);
    CHECK(strcmp(results[0].name, "notes.save[1]+") == 0);
    CHECK(results[1].state == UMI_TEST_STATE_SKIPPED);
    CHECK(results[2].state == UMI_TEST_STATE_FAILED);
    CHECK(results[3].state == UMI_TEST_STATE_NOT_RUN);
    CHECK(results[4].state == UMI_TEST_STATE_SKIPPED && results[4].exit_code == -1);
    CHECK(results[5].state == UMI_TEST_STATE_TIMED_OUT);
    CHECK(UmiCtestResultRequirePassed(&results[6]) == UMI_STATUS_OK);
    CHECK(umi_test_suite_count(suite) == 7U);
    char marker[4096];
    int written = snprintf(marker, sizeof marker, "%s/unexpected.marker", directory);
    CHECK(written > 0 && (size_t)written < sizeof marker);
    FILE *file = fopen(marker, "rb"); CHECK(file == NULL); if (file != NULL) fclose(file);
    /* Cancellation preserves slots and identifiers but starts no further test. */
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    umi_cancellation_token_request(token); options.cancellation = token;
    CHECK(UmiCtestRunSuite(directory, suite, &options, results, 8U, &summary) == UMI_STATUS_CANCELLED);
    CHECK(summary.total == 7U && summary.cancelled == 6U && summary.skipped == 1U);
    CHECK(summary.passed == 0U && summary.failed == 0U && summary.timed_out == 0U);
    CHECK(strcmp(results[0].test_id, "notes.id.literal") == 0);
    CHECK(results[0].exit_code == -1 && results[6].exit_code == -1);
    umi_cancellation_token_destroy(token); options.cancellation = NULL;
    umi_test_suite_destroy(suite); suite = NULL;
    /* Bad bounded storage is rejected before looking for an executable. */
    CHECK(umi_test_suite_create("bad", "Invalid record", &suite) == UMI_STATUS_OK);
    if (suite != NULL) {
        UmiTestCase item; umi_test_case_init(&item, "bad.name", "placeholder");
        memset(item.name, 'x', sizeof item.name);
        CHECK(umi_test_suite_add(suite, &item) == UMI_STATUS_OK);
        CHECK(UmiCtestRunSuite(directory, suite, NULL, results, 8U, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(summary.failed == 1U && results[0].state == UMI_TEST_STATE_NOT_RUN);
        umi_test_suite_destroy(suite);
    }
    CHECK(UmiCtestRunSuite(directory, NULL, NULL, results, 8U, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(summary.total == 0U);
    free(results);
    printf("Suite: %u checks, %u failures\n", checks, failures);
    return failures == 0U ? 0 : 1;
}
