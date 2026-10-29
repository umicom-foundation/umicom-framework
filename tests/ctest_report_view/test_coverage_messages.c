/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_messages.c
 *
 * PURPOSE:
 *   Preserve recorded result messages in complete and filtered coverage pages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/testing/ctest_coverage.h"
#include "umicom/testing/ctest_coverage_view.h"

typedef struct MessageCase {
    const char *name, *status, *detail, *expected;
    UmiCtestCoverageOutcome outcome;
} MessageCase;

static const MessageCase cases[] = {
    {"passed", "run", "", "not recorded", UMI_CTEST_COVERAGE_PASSED},
    {"failed", "fail", "<failure message='Failed'/>", "Failed", UMI_CTEST_COVERAGE_FAILED},
    {"timeout", "fail", "<failure message='Timeout'/>", "Timeout", UMI_CTEST_COVERAGE_TIMED_OUT},
    {"skip-return", "notrun", "<skipped message='SKIP_RETURN_CODE=77'/>",
        "SKIP_RETURN_CODE=77", UMI_CTEST_COVERAGE_SKIPPED},
    {"skip-pattern", "notrun", "<skipped message='SKIP_REGULAR_EXPRESSION_MATCHED'/>",
        "SKIP_REGULAR_EXPRESSION_MATCHED", UMI_CTEST_COVERAGE_SKIPPED},
    {"disabled", "disabled", "", "Test is disabled.", UMI_CTEST_COVERAGE_DISABLED},
    {"not-run", "notrun", "<skipped message='Unable to find executable'/>",
        "Unable to find executable", UMI_CTEST_COVERAGE_NOT_RUN},
    {"empty-message", "fail", "<failure message=''/>",
        "not recorded", UMI_CTEST_COVERAGE_FAILED},
    {"unicode", "fail", "<failure message='Le fichier caf&#233; est absent'/>",
        "Le fichier caf\xc3\xa9 est absent", UMI_CTEST_COVERAGE_FAILED},
    {"controls", "fail", "<failure message='Denied&#10;[passed] forged&#13;&#9;C:\\work&#127;'/>",
        "Denied\\x0A[passed] forged\\x0D\\x09C:\\x5Cwork\\x7F", UMI_CTEST_COVERAGE_FAILED},
    {"markup", "fail", "<failure message='&lt;button&gt; &amp; &quot;quoted&quot;'/>",
        "<button> & \"quoted\"", UMI_CTEST_COVERAGE_FAILED},
    {"limit", "fail", "", "", UMI_CTEST_COVERAGE_FAILED},
    {"stale", "fail", "<failure message='Failed'/>", NULL, UMI_CTEST_COVERAGE_SOURCE_MISMATCH},
    {"missing-host", "fail", "<failure message='Failed'/>", NULL, UMI_CTEST_COVERAGE_MISSING_HOST},
    {"missing-test", "fail", "<failure message='Failed'/>", NULL, UMI_CTEST_COVERAGE_MISSING_TEST},
    {"filtered-out", "fail", "<failure message='Failed'/>", "Failed", UMI_CTEST_COVERAGE_FAILED}
};

static UmiStatus Format(const UmiCtestCoverageReview *review,
    const UmiCtestCoverageView *view, int filtered,
    char *output, size_t capacity, size_t *required)
{
    return filtered ? UmiCtestCoverageViewFormat(view, 0U, 1U, output, capacity, required) :
        UmiCtestCoverageReviewFormat(review, 0U, 1U, output, capacity, required);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const MessageCase *scenario = NULL;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(argv[1], cases[i].name) == 0) scenario = &cases[i];
    CHECK(scenario != NULL);

    /* Import real CTest shapes, rather than injecting an internal row. This
     * covers decoding, copied review ownership and the shared presentation. */
    char maximum[256], detail[320], xml[2048];
    memset(maximum, 'Q', sizeof(maximum) - 1U);
    maximum[sizeof(maximum) - 1U] = '\0';
    const char *body = scenario->detail, *expected = scenario->expected;
    if (strcmp(scenario->name, "limit") == 0) {
        int n = snprintf(detail, sizeof(detail), "<failure message='%s'/>", maximum);
        CHECK(n > 0 && (size_t)n < sizeof(detail));
        body = detail;
        expected = maximum;
    }
    int n = snprintf(xml, sizeof(xml),
        "<testsuite tests='1'><testcase name='example.check' status='%s' time='0.125'>"
        "%s</testcase></testsuite>", scenario->status, body);
    CHECK(n > 0 && (size_t)n < sizeof(xml));
    UmiCtestReport *report = NULL;
    OK(UmiCtestReportImport(xml, (size_t)n, &report));
    UmiCtestCoverageHost host = {
        .host = "desktop", .configuration = "Debug",
        .source_identity = "source-A", .report = report
    };
    UmiCtestCoverageRequirement requirement = {
        .name = "example.check", .host = "desktop",
        .configuration = "Debug", .capability = "native fixture"
    };
    if (strcmp(scenario->name, "stale") == 0) strcpy(host.source_identity, "source-B");
    if (strcmp(scenario->name, "missing-host") == 0) strcpy(requirement.host, "other");
    if (strcmp(scenario->name, "missing-test") == 0) strcpy(requirement.name, "absent");
    UmiCtestCoverageReview *review = NULL;
    OK(UmiCtestCoverageReviewCreate("source-A", &requirement, 1U, &host, 1U, &review));
    UmiCtestReportDestroy(report);
    memset(xml, 'X', sizeof(xml)); /* No later formatting may borrow input. */
    memset(&requirement, 0, sizeof(requirement));
    const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, 0U);
    CHECK(row != NULL && row->outcome == scenario->outcome);
    CHECK(row->has_observation == (expected != NULL));

    UmiCtestCoverageFilter filter;
    UmiCtestCoverageFilterInit(&filter);
    filter.outcomes = UMI_CTEST_COVERAGE_VIEW_ALL;
    int excluded = strcmp(scenario->name, "filtered-out") == 0;
    if (excluded) strcpy(filter.text_contains, "unrelated requirement");
    UmiCtestCoverageView *view = NULL;
    OK(UmiCtestCoverageViewCreate(review, &filter, &view));
    UmiStatus gate = UmiCtestCoverageRequirePassed(review);
    CHECK(gate == (scenario->outcome == UMI_CTEST_COVERAGE_PASSED ?
        UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
    for (int filtered = 0; filtered <= 1; ++filtered) {
        size_t bytes = 0U, reported = 0U;
        OK(Format(review, view, filtered, NULL, 0U, &bytes));
        CHECK(bytes > 0U);
        char *text = malloc(bytes + 1U);
        CHECK(text != NULL);
        memset(text, 'Z', bytes + 1U);
        CHECK(Format(review, view, filtered, text, bytes, &reported) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(reported == bytes);
        for (size_t i = 0U; i <= bytes; ++i) CHECK(text[i] == 'Z');
        OK(Format(review, view, filtered, text, bytes + 1U, &reported));
        CHECK(reported == bytes && strlen(text) == bytes);
        const char *message = strstr(text, "\n  Saved result message: ");
        if (expected == NULL || (filtered && excluded)) CHECK(message == NULL);
        else {
            CHECK(message != NULL);
            message += strlen("\n  Saved result message: ");
            CHECK(strncmp(message, expected, strlen(expected)) == 0);
            CHECK(message[strlen(expected)] == '\n');
            CHECK(strstr(text, "\n[passed] forged") == NULL);
            CHECK(strstr(text, "\n  Saved duration: 125 ms") != NULL);
        }
        if (filtered && excluded)
            CHECK(strstr(text, "No matching rows. This is not a passing qualification.") != NULL);
        if (expected == NULL)
            CHECK(strstr(text, "No matching observation for the requested source.") != NULL);
        free(text);
    }
    CHECK(UmiCtestCoverageRequirePassed(review) == gate);
    UmiCtestCoverageViewDestroy(view);
    UmiCtestCoverageReviewDestroy(review);
    return 0;
}
