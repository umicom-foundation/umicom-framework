/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reuse/test_receipts.c
 *
 * PURPOSE:
 *   Verify selective retries and outcome persistence without child processes.
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
#define A "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define B "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
#define C "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
#define HEAD "UMICOM-TEST-REUSE 1 " A "\n"
static const char current_text[] = HEAD "alpha\t" A "\t0\nbeta\t" B "\t0\n";
static const char prior_text[] = HEAD "alpha\t" A "\t2\nbeta\t" B "\t3\n";
static int Import(const char *text, UmiTestReuseState **state)
{ return UmiTestReuseImport(text, strlen(text), state) == UMI_STATUS_OK; }

static int CheckLineEndings(void)
{
    static const char crlf[] =
        "UMICOM-TEST-REUSE 1 " A "\r\nalpha\t" A "\t0\r\nbeta\t" B "\t0\r\n";
    static const char mixed[] =
        "UMICOM-TEST-REUSE 1 " A "\r\nalpha\t" A "\t0\nbeta\t" B "\t0\r\n";
    static const char *const accepted[] = {current_text, crlf, mixed};
    for (size_t i = 0U; i < sizeof(accepted) / sizeof(accepted[0]); ++i) {
        UmiTestReuseState *state = NULL;
        CHECK(Import(accepted[i], &state));
        size_t required = 0U;
        char exported[512];
        CHECK(UmiTestReuseExport(state, exported, sizeof(exported), &required) == UMI_STATUS_OK);
        CHECK(strcmp(exported, current_text) == 0 && required == sizeof(current_text));
        CHECK(memchr(exported, '\r', required) == NULL);
        UmiTestReuseDestroy(state);
    }
    /* Malformed terminators must not repair a damaged name, digest or record.
     * Incomplete input cannot publish even the valid prefix into the output. */
    static const char *const rejected[] = {
        "UMICOM-TEST-REUSE 1 " A "\r",
        "UMICOM-TEST-REUSE 1 " A "\r\r\n",
        HEAD "al\rpha\t" A "\t0\r\n",
        HEAD "alpha\t" A "\r\t0\n",
        HEAD "alpha\t" A "\t0\r",
        HEAD "alpha\t" A "\t0\r\n\r\n",
        HEAD "alpha\t" A "\t0\r\nalpha\t" B "\t0\n",
        HEAD "alpha\t" A "\t9\r\n"
    };
    for (size_t i = 0U; i < sizeof(rejected) / sizeof(rejected[0]); ++i) {
        UmiTestReuseState *state = NULL;
        CHECK(!Import(rejected[i], &state) && state == NULL);
    }
    UmiTestReuseState *state = NULL;
    CHECK(Import("UMICOM-TEST-REUSE 1 " A "\r\n", &state));
    CHECK(UmiTestReuseCount(state) == 0U);
    UmiTestReuseDestroy(state); state = NULL;
    char binary[sizeof(crlf)];
    memcpy(binary, crlf, sizeof(binary));
    binary[sizeof(binary) - 5U] = '\0';
    CHECK(UmiTestReuseImport(binary, sizeof(binary) - 1U, &state) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(state == NULL);
    CHECK(UmiTestReuseImport(crlf, UMI_TEST_REUSE_MAX_BYTES + 1U, &state) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(state == NULL);
    return 0;
}

int main(void)
{
    CHECK(CheckLineEndings() == 0);
    UmiTestReuseState *current = NULL, *prior = NULL, *next = NULL;
    CHECK(Import(current_text, &current) && Import(prior_text, &prior));
    CHECK(UmiTestReusePrepare(current, prior, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseCount(next) == 2U);
    CHECK(strcmp(UmiTestReuseName(next, 0U), "alpha") == 0);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_PASSED);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_NOT_RUN);
    /* An interrupted attempt keeps the failed case due, independently of
     * the unaffected passing case. This does not claim the suite has passed. */
    UmiTestReuseState *retry = NULL;
    CHECK(UmiTestReusePrepare(current, next, &retry) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(retry, 0U) == UMI_TEST_STATE_PASSED);
    CHECK(UmiTestReuseOutcome(retry, 1U) == UMI_TEST_STATE_NOT_RUN);
    UmiTestReuseDestroy(retry);
    const char *pass = "<testsuite tests='1'><testcase name='beta' status='run' time='0.1'/></testsuite>";
    CHECK(UmiTestReuseRecord(next, pass, strlen(pass)) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_PASSED);
    const char *skip = "<testsuite tests='1'><testcase name='beta' status='notrun' time='0'><skipped message='SKIP_RETURN_CODE=77'/></testcase></testsuite>";
    CHECK(UmiTestReuseRecord(next, skip, strlen(skip)) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_SKIPPED);
    const char *timeout = "<testsuite tests='1'><testcase name='beta' status='fail' time='1'><failure message='Timeout'/></testcase></testsuite>";
    CHECK(UmiTestReuseRecord(next, timeout, strlen(timeout)) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_TIMED_OUT);
    const char *malformed = "<testsuite tests='2'><testcase name='beta' status='run' time='0'/></testsuite>";
    CHECK(UmiTestReuseRecord(next, malformed, strlen(malformed)) != UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_TIMED_OUT);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_PASSED);
    const char *disabled = "<testsuite tests='1'><testcase name='beta' status='disabled' time='0'/></testsuite>";
    CHECK(UmiTestReuseRecord(next, disabled, strlen(disabled)) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 1U) == UMI_TEST_STATE_SKIPPED);
    UmiTestReuseDestroy(next); next = NULL;
    UmiTestReuseState *changed = NULL;
    CHECK(Import(HEAD "alpha\t" C "\t0\nbeta\t" B "\t0\n", &changed));
    CHECK(UmiTestReusePrepare(changed, prior, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_NOT_RUN);
    UmiTestReuseDestroy(next); next = NULL; UmiTestReuseDestroy(changed); changed = NULL;
    CHECK(Import("UMICOM-TEST-REUSE 1 " C "\nalpha\t" A "\t0\n", &changed));
    CHECK(UmiTestReusePrepare(changed, prior, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_NOT_RUN);
    UmiTestReuseDestroy(next); next = NULL; UmiTestReuseDestroy(changed); changed = NULL;
    /* A normal CTest failure overrides an older automatic passing receipt. */
    CHECK(Import(HEAD "alpha\t" A "\t3\n", &changed));
    CHECK(UmiTestReusePrepare(changed, prior, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_NOT_RUN);
    UmiTestReuseDestroy(next); next = NULL; UmiTestReuseDestroy(changed); changed = NULL;
    CHECK(UmiTestReusePrepare(current, NULL, &next) == UMI_STATUS_OK);
    CHECK(UmiTestReuseOutcome(next, 0U) == UMI_TEST_STATE_NOT_RUN);
    size_t required = 0U;
    CHECK(UmiTestReuseExport(next, NULL, 0U, &required) == UMI_STATUS_OK);
    char *saved = malloc(required); CHECK(saved != NULL);
    CHECK(UmiTestReuseExport(next, saved, required, &required) == UMI_STATUS_OK);
    CHECK(strlen(saved) + 1U == required && Import(saved, &changed));
    free(saved);
    char small[4] = "old"; size_t untouched = 91U;
    CHECK(UmiTestReuseExport(next, small, sizeof(small), &untouched) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(small, "old") == 0 && untouched == 91U);
    UmiTestReuseDestroy(changed); changed = NULL;
    CHECK(!Import(HEAD "alpha\t" A "\t0\nalpha\t" B "\t0\n", &changed));
    CHECK(changed == NULL);
    CHECK(!Import(HEAD "alpha\t" A "\t9\n", &changed));
    CHECK(!Import(HEAD "alpha\tbad\t0\n", &changed));
    CHECK(!Import("UMICOM-TEST-REUSE 2 " A "\n", &changed));
    CHECK(Import(HEAD, &changed) && UmiTestReuseCount(changed) == 0U);
    UmiTestReuseDestroy(changed); UmiTestReuseDestroy(next);
    UmiTestReuseDestroy(prior); UmiTestReuseDestroy(current);
    return 0;
}
