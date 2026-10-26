/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_text.c
 *
 * PURPOSE:
 *   Exercise exact saved coverage text, bounds and immutable review ownership.
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

static char *Format(const UmiCtestCoverageReview *review, size_t offset, size_t limit)
{
    size_t required = 0;
    OK(UmiCtestCoverageReviewFormat(review, offset, limit, NULL, 0, &required));
    char *text = malloc(required + 1U);
    CHECK(text);
    OK(UmiCtestCoverageReviewFormat(review, offset, limit, text, required + 1U, &required));
    CHECK(strlen(text) == required);
    return text;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"outcomes", "first", "last", "empty", "query", "short", "exact",
        "offset", "limit", "null", "unicode", "escape", "ownership", "source-mismatch"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *test = argv[1];
    UmiCtestReport *report = Mixed();
    UmiCtestCoverageHost hosts[2] = {
        {.host = "windows", .configuration = "Debug", .source_identity = "source-A", .report = report},
        {.host = "stale", .configuration = "Debug", .source_identity = "source-B", .report = report}
    };
    const char *names[] = {"z.pass", "b.fail", "d.timeout", "c.skip", "e.disabled", "a.missing",
        "absent", "host-only", "z.pass"};
    UmiCtestCoverageRequirement requirements[9] = {0};
    for (size_t i = 0; i < 9; ++i) {
        strcpy(requirements[i].name, names[i]);
        strcpy(requirements[i].host, i == 7 ? "missing" : i == 8 ? "stale" : "windows");
        strcpy(requirements[i].configuration, "Debug");
        strcpy(requirements[i].capability, "native display");
    }
    if (!strcmp(test, "unicode")) strcpy(requirements[7].capability, "écran");
    if (!strcmp(test, "escape")) strcpy(requirements[7].capability, "C:\\tools\\report");
    size_t count = !strcmp(test, "empty") ? 0U : 9U;
    UmiCtestCoverageReview *review = NULL;
    OK(UmiCtestCoverageReviewCreate("source-A", requirements, count, hosts, 2, &review));
    UmiCtestCoverageSummary before, after;
    memset(&before, 0, sizeof before); memset(&after, 0, sizeof after);
    OK(UmiCtestCoverageReviewRead(review, &before));
    UmiCtestReportDestroy(report); /* The formatter must use the owned review. */
    memset(requirements, 0, sizeof requirements);
    char output[16]; memset(output, 'Q', sizeof output);
    size_t required = 987U;
    if (!strcmp(test, "null")) {
        CHECK(UmiCtestCoverageReviewFormat(NULL, 0, 1, output, sizeof output, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageReviewFormat(review, 0, 1, NULL, 1, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageReviewFormat(review, 0, 1, output, sizeof output, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 987U && output[0] == 'Q');
    } else if (!strcmp(test, "offset")) {
        CHECK(UmiCtestCoverageReviewFormat(review, 9, 1, output, sizeof output, &required) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiCtestCoverageReviewFormat(review, SIZE_MAX, 1, output, sizeof output, &required) == UMI_STATUS_NOT_FOUND);
        CHECK(required == 987U && output[0] == 'Q');
    } else if (!strcmp(test, "limit")) {
        CHECK(UmiCtestCoverageReviewFormat(review, 0, 0, output, sizeof output, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageReviewFormat(review, 0, 129, output, sizeof output, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 987U && output[0] == 'Q');
    } else if (!strcmp(test, "short")) {
        CHECK(UmiCtestCoverageReviewFormat(review, 0, 1, output, sizeof output, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(required > sizeof output);
        for (size_t i = 0; i < sizeof output; ++i) CHECK(output[i] == 'Q');
    } else {
        size_t offset = !strcmp(test, "last") ? 8U : 0U;
        size_t limit = !strcmp(test, "first") || !strcmp(test, "last") ? 1U : 128U;
        char *text = Format(review, offset, limit);
        CHECK(strstr(text, "no tests were run and source identities are not verified."));
        CHECK(strstr(text, "Requested source: source-A\n"));
        CHECK(strstr(text, "Only listed requirements are reviewed."));
        if (!strcmp(test, "first")) {
            CHECK(strstr(text, "Showing 1..1 of 9"));
            CHECK(strstr(text, "[passed] windows / Debug : z.pass"));
            CHECK(!strstr(text, "[failed]"));
        } else if (!strcmp(test, "last")) {
            CHECK(strstr(text, "Showing 9..9 of 9"));
            CHECK(strstr(text, "[source mismatch] stale / Debug : z.pass"));
            CHECK(!strstr(text, "[passed]"));
        } else if (!strcmp(test, "empty")) {
            CHECK(strstr(text, "Showing 0..0 of 0"));
            CHECK(strstr(text, "No requirements: empty coverage cannot pass."));
        } else {
            CHECK(strstr(text, "Requirements: 9 | Passed: 1 | Outstanding: 8"));
            CHECK(strstr(text, "[failed] windows / Debug : b.fail"));
            CHECK(strstr(text, "[timed out] windows / Debug : d.timeout"));
            CHECK(strstr(text, "[skipped] windows / Debug : c.skip"));
            CHECK(strstr(text, "[disabled] windows / Debug : e.disabled"));
            CHECK(strstr(text, "[not run] windows / Debug : a.missing"));
            CHECK(strstr(text, "[missing test] windows / Debug : absent"));
            CHECK(strstr(text, "[missing host] missing / Debug : host-only"));
            CHECK(strstr(text, "[source mismatch] stale / Debug : z.pass"));
            CHECK(strstr(text, "Observed source: source-B"));
            CHECK(strstr(text, "No matching observation for the requested source."));
            if (!strcmp(test, "unicode")) CHECK(strstr(text, "Requires: écran"));
            if (!strcmp(test, "escape")) CHECK(strstr(text, "Requires: C:\\x5Ctools\\x5Creport"));
        }
        if (!strcmp(test, "exact")) {
            size_t bytes = strlen(text);
            char *exact = malloc(bytes + 1U); CHECK(exact);
            memset(exact, 'Q', bytes + 1U);
            CHECK(UmiCtestCoverageReviewFormat(review, offset, limit, exact, bytes, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
            for (size_t i = 0; i <= bytes; ++i) CHECK(exact[i] == 'Q');
            OK(UmiCtestCoverageReviewFormat(review, offset, limit, exact, bytes + 1U, &required));
            CHECK(!strcmp(exact, text)); free(exact);
        }
        free(text);
    }
    OK(UmiCtestCoverageReviewRead(review, &after));
    CHECK(memcmp(&before, &after, sizeof before) == 0);
    UmiCtestCoverageReviewDestroy(review);
    return 0;
}
