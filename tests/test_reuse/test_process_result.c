/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reuse/test_process_result.c
 *
 * PURPOSE:
 *   Keep unsuccessful runner observations due without losing historical exclusions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/reuse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define DIGEST "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define HEAD "UMICOM-TEST-REUSE 1 " DIGEST "\n"
#define ROW(name, state) name "\t" DIGEST "\t" state "\n"
#define SUITE(body) "<testsuite tests='1'>" body "</testsuite>"
#define PASS(name) "<testcase name='" name "' status='run' time='0.1'/>"
#define FAILED(name) "<testcase name='" name "' status='fail' time='0'><failure message='assertion'/></testcase>"

typedef struct Case {
    const char *name;
    const char *xml;
    bool runnerSucceeded;
    bool accepted;
    UmiTestState alpha;
    UmiTestState beta;
} Case;
static const Case cases[] = {
    {"success", SUITE(PASS("beta")), true, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_PASSED},
    {"runner-failed", SUITE(PASS("beta")), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"reported-history", SUITE(PASS("alpha")), false, true, UMI_TEST_STATE_NOT_RUN, UMI_TEST_STATE_NOT_RUN},
    {"unreported-history", "<testsuite tests='0'/>", false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"assertion", SUITE(FAILED("beta")), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_FAILED},
    {"assertion-success-status", SUITE(FAILED("beta")), true, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_FAILED},
    {"timeout", SUITE("<testcase name='beta' status='fail' time='1'><failure message='Timeout'/></testcase>"), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_TIMED_OUT},
    {"skip-return", SUITE("<testcase name='beta' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase>"), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_SKIPPED},
    {"skip-pattern", SUITE("<testcase name='beta' status='notrun' time='0'><skipped message='SKIP_REGULAR_EXPRESSION_MATCHED'/></testcase>"), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_SKIPPED},
    {"disabled", SUITE("<testcase name='beta' status='disabled' time='0'/>"), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_SKIPPED},
    {"not-run", SUITE("<testcase name='beta' status='notrun' time='0'><skipped message='Unable to find executable'/></testcase>"), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"unknown", SUITE(PASS("outside")), false, true, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"mixed", "<testsuite tests='2'>" PASS("alpha") FAILED("beta") "</testsuite>", false, true, UMI_TEST_STATE_NOT_RUN, UMI_TEST_STATE_FAILED},
    {"both-pass", "<testsuite tests='2'>" PASS("alpha") PASS("beta") "</testsuite>", false, true, UMI_TEST_STATE_NOT_RUN, UMI_TEST_STATE_NOT_RUN},
    {"bad-count", "<testsuite tests='2'>" PASS("beta") "</testsuite>", false, false, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"truncated", "<testsuite tests='1'>" PASS("beta"), false, false, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"duplicate", "<testsuite tests='2'>" PASS("beta") PASS("beta") "</testsuite>", false, false, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN},
    {"empty", "", false, false, UMI_TEST_STATE_PASSED, UMI_TEST_STATE_NOT_RUN}
};
static int CheckCase(const Case *scenario)
{
    const char *initial = HEAD ROW("alpha", "2") ROW("beta", "0");
    const char *manifest = HEAD ROW("alpha", "0") ROW("beta", "0");
    UmiTestReuseState *state = NULL, *current = NULL, *next = NULL;
    CHECK(UmiTestReuseImport(initial, strlen(initial), &state) == UMI_STATUS_OK);
    char before[512], after[512];
    size_t size = 0U;
    CHECK(UmiTestReuseExport(state, before, sizeof(before), &size) == UMI_STATUS_OK);
    UmiStatus result = UmiTestReuseRecordProcess(state, scenario->xml,
        strlen(scenario->xml), scenario->runnerSucceeded);
    CHECK((result == UMI_STATUS_OK) == scenario->accepted);
    CHECK(UmiTestReuseCount(state) == 2U);
    CHECK(UmiTestReuseOutcome(state, 0U) == scenario->alpha);
    CHECK(UmiTestReuseOutcome(state, 1U) == scenario->beta);
    CHECK(UmiTestReuseExport(state, after, sizeof(after), &size) == UMI_STATUS_OK);
    if (!scenario->accepted) CHECK(strcmp(before, after) == 0);
    /* A second preparation consumes only qualified historical passes. Failure
     * details remain in the first receipt/report while unsuccessful cases retry. */
    CHECK(UmiTestReuseImport(manifest, strlen(manifest), &current) == UMI_STATUS_OK);
    CHECK(UmiTestReusePrepare(current, state, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 0U) == (scenario->alpha == UMI_TEST_STATE_PASSED ?
        UMI_TEST_STATE_PASSED : UMI_TEST_STATE_NOT_RUN));
    CHECK(UmiTestReuseOutcome(next, 1U) == (scenario->beta == UMI_TEST_STATE_PASSED ?
        UMI_TEST_STATE_PASSED : UMI_TEST_STATE_NOT_RUN));
    UmiTestReuseDestroy(next); UmiTestReuseDestroy(current); UmiTestReuseDestroy(state);
    return 0;
}
static int CheckBoundary(const char *name)
{
    const char *text = HEAD ROW("beta", "0");
    const char *pass = SUITE(PASS("beta"));
    UmiTestReuseState *state = NULL;
    CHECK(UmiTestReuseImport(text, strlen(text), &state) == UMI_STATUS_OK);
    if (strcmp(name, "legacy") == 0) {
        CHECK(UmiTestReuseRecord(state, pass, strlen(pass)) == UMI_STATUS_OK);
        CHECK(UmiTestReuseOutcome(state, 0U) == UMI_TEST_STATE_PASSED);
    } else if (strcmp(name, "null-state") == 0) {
        CHECK(UmiTestReuseRecordProcess(NULL, pass, strlen(pass), false) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "null-report") == 0) {
        CHECK(UmiTestReuseRecordProcess(state, NULL, 1U, false) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestReuseOutcome(state, 0U) == UMI_TEST_STATE_NOT_RUN);
    } else if (strcmp(name, "retry-success") == 0) {
        CHECK(UmiTestReuseRecordProcess(state, pass, strlen(pass), false) == UMI_STATUS_OK);
        CHECK(UmiTestReuseOutcome(state, 0U) == UMI_TEST_STATE_NOT_RUN);
        CHECK(UmiTestReuseRecordProcess(state, pass, strlen(pass), true) == UMI_STATUS_OK);
        CHECK(UmiTestReuseOutcome(state, 0U) == UMI_TEST_STATE_PASSED);
    } else {
        UmiTestReuseDestroy(state);
        fprintf(stderr, "Unknown process result case: %s\n", name);
        return 2;
    }
    UmiTestReuseDestroy(state);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(argv[1], cases[i].name) == 0) return CheckCase(&cases[i]);
    return CheckBoundary(argv[1]);
}
