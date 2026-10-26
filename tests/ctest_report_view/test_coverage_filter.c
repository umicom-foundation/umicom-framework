/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage_filter.c
 *
 * PURPOSE:
 *   Check filtered host coverage without allowing hidden failures to satisfy qualification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "umicom/testing/ctest_coverage_view.h"
static UmiCtestCoverageRequirement Requirement(const char *name, const char *host, const char *configuration) {
    UmiCtestCoverageRequirement row = {0};
    (void)snprintf(row.name, sizeof row.name, "%s", name);
    (void)snprintf(row.host, sizeof row.host, "%s", host);
    (void)snprintf(row.configuration, sizeof row.configuration, "%s", configuration);
    strcpy(row.capability, "Native display");
    return row;
}
static UmiCtestCoverageReview *Review(bool empty) {
    UmiCtestReport *report = Mixed();
    UmiCtestCoverageHost hosts[2] = {0};
    strcpy(hosts[0].host, "Windows"); strcpy(hosts[0].configuration, "Debug");
    strcpy(hosts[0].source_identity, "source-A"); hosts[0].report = report;
    strcpy(hosts[1].host, "Stale"); strcpy(hosts[1].configuration, "Release");
    strcpy(hosts[1].source_identity, "source-B"); hosts[1].report = report;
    const char *names[] = {"z.pass", "b.fail", "d.timeout", "c.skip", "e.disabled", "a.missing"};
    UmiCtestCoverageRequirement requirements[9];
    for (size_t i = 0U; i < 6U; ++i) requirements[i] = Requirement(names[i], "Windows", "Debug");
    strcpy(requirements[4].capability, "GPU compute");
    requirements[6] = Requirement("no-test", "Windows", "Debug");
    requirements[7] = Requirement("absent-host", "Remote", "Debug");
    requirements[8] = Requirement("stale-source", "Stale", "Release");
    UmiCtestCoverageReview *review = NULL;
    OK(UmiCtestCoverageReviewCreate("source-A", requirements, empty ? 0U : 9U, hosts, 2U, &review));
    UmiCtestReportDestroy(report); /* Reviews own copied evidence. Views borrow that review. */
    return review;
}
int main(int argc, char **argv) {
    const char *cases[] = {"default", "all", "passed-gate", "none", "empty", "name", "capability", "host",
        "configuration", "combined", "literal", "non-ascii", "order-name", "order-host", "order-outcome",
        "filter-copy", "outcome-0", "outcome-1", "outcome-2", "outcome-3", "outcome-4", "outcome-5",
        "outcome-6", "outcome-7", "outcome-8", "invalid-mask", "invalid-order", "unterminated", "invalid-utf8",
        "controls", "invalid-api", "format", "short", "paging", "empty-page"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *mode = argv[1];
    UmiCtestCoverageReview *review = Review(!strcmp(mode, "empty"));
    UmiCtestCoverageFilter filter;
    UmiCtestCoverageFilterInit(&filter);
    CHECK(filter.outcomes == UMI_CTEST_COVERAGE_VIEW_OUTSTANDING);
    size_t expected = !strcmp(mode, "empty") ? 0U : 8U;
    if (!strcmp(mode, "all")) { filter.outcomes = UMI_CTEST_COVERAGE_VIEW_ALL; expected = 9U; }
    if (!strcmp(mode, "passed-gate")) { filter.outcomes = 1U; expected = 1U; }
    if (!strcmp(mode, "none") || !strcmp(mode, "empty-page")) { filter.outcomes = 0U; expected = 0U; }
    if (!strcmp(mode, "name")) { strcpy(filter.text_contains, "B.FAIL"); expected = 1U; }
    if (!strcmp(mode, "capability")) { strcpy(filter.text_contains, "gpu"); expected = 1U; }
    if (!strcmp(mode, "host")) { strcpy(filter.host_contains, "win"); expected = 6U; }
    if (!strcmp(mode, "configuration")) { strcpy(filter.configuration_contains, "RELEASE"); expected = 1U; }
    if (!strcmp(mode, "combined")) {
        strcpy(filter.text_contains, "native"); strcpy(filter.host_contains, "win");
        strcpy(filter.configuration_contains, "debug"); expected = 5U;
    }
    if (!strcmp(mode, "literal")) { strcpy(filter.text_contains, ".*"); expected = 0U; }
    if (!strcmp(mode, "non-ascii")) { strcpy(filter.text_contains, "caf\xc3\xa9"); expected = 0U; }
    if (!strcmp(mode, "order-name")) filter.order = UMI_CTEST_COVERAGE_NAME_ORDER;
    if (!strcmp(mode, "order-host")) filter.order = UMI_CTEST_COVERAGE_HOST_ORDER;
    if (!strcmp(mode, "order-outcome")) filter.order = UMI_CTEST_COVERAGE_OUTCOME_ORDER;
    if (!strncmp(mode, "outcome-", 8U)) { filter.outcomes = 1U << (unsigned)(mode[8] - '0'); expected = 1U; }
    if (!strcmp(mode, "invalid-mask")) filter.outcomes = UMI_CTEST_COVERAGE_VIEW_ALL + 1U;
    if (!strcmp(mode, "invalid-order")) filter.order = (UmiCtestCoverageOrder)99;
    if (!strcmp(mode, "unterminated")) memset(filter.text_contains, 'x', sizeof filter.text_contains);
    if (!strcmp(mode, "invalid-utf8")) { filter.host_contains[0] = (char)0xFF; filter.host_contains[1] = '\0'; }
    if (!strcmp(mode, "controls")) strcpy(filter.configuration_contains, "Debug\n");
    UmiCtestCoverageView *view = NULL;
    if (!strcmp(mode, "invalid-api")) {
        UmiCtestCoverageViewSummary untouched; memset(&untouched, 0xA5, sizeof untouched);
        UmiCtestCoverageViewSummary copy = untouched;
        CHECK(UmiCtestCoverageViewCreate(NULL, &filter, &view) == UMI_STATUS_INVALID_ARGUMENT && view == NULL);
        CHECK(UmiCtestCoverageViewCreate(review, NULL, &view) == UMI_STATUS_INVALID_ARGUMENT && view == NULL);
        CHECK(UmiCtestCoverageViewCreate(review, &filter, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageViewRead(NULL, &untouched) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&untouched, &copy, sizeof copy) == 0);
        CHECK(UmiCtestCoverageViewAt(NULL, 0U) == NULL && UmiCtestCoverageViewSourceIdentity(NULL) == NULL);
        UmiCtestCoverageFilterInit(NULL); UmiCtestCoverageViewDestroy(NULL);
    } else if (!strcmp(mode, "invalid-mask") || !strcmp(mode, "invalid-order") || !strcmp(mode, "unterminated") ||
        !strcmp(mode, "invalid-utf8") || !strcmp(mode, "controls")) {
        CHECK(UmiCtestCoverageFilterValidate(&filter) != UMI_STATUS_OK);
        CHECK(UmiCtestCoverageViewCreate(review, &filter, &view) != UMI_STATUS_OK && view == NULL);
    } else {
        OK(UmiCtestCoverageViewCreate(review, &filter, &view));
        if (!strcmp(mode, "filter-copy")) memset(&filter, 0xFF, sizeof filter);
        UmiCtestCoverageViewSummary summary;
        OK(UmiCtestCoverageViewRead(view, &summary)); CHECK(summary.matches == expected);
        CHECK(summary.source.requirements == (!strcmp(mode, "empty") ? 0U : 9U));
        CHECK(summary.source.outstanding == (!strcmp(mode, "empty") ? 0U : 8U));
        CHECK(UmiCtestCoverageRequirePassed(review) != UMI_STATUS_OK);
        CHECK(!strcmp(UmiCtestCoverageViewSourceIdentity(view), "source-A"));
        CHECK(UmiCtestCoverageViewAt(view, expected) == NULL);
        if (!strcmp(mode, "filter-copy")) {
            CHECK(summary.filter.outcomes == UMI_CTEST_COVERAGE_VIEW_OUTSTANDING);
            CHECK(summary.filter.order == UMI_CTEST_COVERAGE_REQUIREMENT_ORDER);
            CHECK(summary.filter.text_contains[0] == '\0' && summary.filter.host_contains[0] == '\0');
        }
        if (!strcmp(mode, "passed-gate")) CHECK(summary.outstanding_matches == 0U && summary.source.outstanding == 8U);
        if (!strncmp(mode, "outcome-", 8U)) CHECK(UmiCtestCoverageViewAt(view, 0U)->outcome == (UmiCtestCoverageOutcome)(mode[8] - '0'));
        if (!strcmp(mode, "order-name")) {
            for (size_t i = 1U; i < expected; ++i)
                CHECK(strcmp(UmiCtestCoverageViewAt(view, i - 1U)->requirement.name, UmiCtestCoverageViewAt(view, i)->requirement.name) <= 0);
        }
        if (!strcmp(mode, "order-host")) CHECK(!strcmp(UmiCtestCoverageViewAt(view, 0U)->requirement.host, "Remote"));
        if (!strcmp(mode, "order-outcome")) {
            for (size_t i = 1U; i < expected; ++i)
                CHECK(UmiCtestCoverageViewAt(view, i - 1U)->outcome <= UmiCtestCoverageViewAt(view, i)->outcome);
        }
        size_t needed = 0U;
        OK(UmiCtestCoverageViewFormat(view, 0U, 3U, NULL, 0U, &needed));
        char *page = malloc(needed + 1U); CHECK(page); memset(page, 'X', needed + 1U);
        if (!strcmp(mode, "short")) {
            CHECK(UmiCtestCoverageViewFormat(view, 0U, 3U, page, needed, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
            for (size_t i = 0U; i <= needed; ++i) CHECK(page[i] == 'X');
        } else {
            OK(UmiCtestCoverageViewFormat(view, 0U, 3U, page, needed + 1U, &needed));
            CHECK(strstr(page, "Filtered rows cannot certify the whole review"));
            if (expected == 0U) CHECK(strstr(page, "No matching rows"));
            if (!strcmp(mode, "format")) CHECK(strstr(page, "Whole review: 9 requirements | Passed: 1 | Outstanding: 8"));
        }
        free(page);
        if (!strcmp(mode, "paging")) {
            size_t sentinel = 17U;
            CHECK(UmiCtestCoverageViewFormat(view, 8U, 1U, NULL, 0U, &sentinel) == UMI_STATUS_NOT_FOUND && sentinel == 17U);
            CHECK(UmiCtestCoverageViewFormat(view, 0U, 129U, NULL, 0U, &sentinel) == UMI_STATUS_INVALID_ARGUMENT && sentinel == 17U);
            OK(UmiCtestCoverageViewFormat(view, 7U, 1U, NULL, 0U, &needed));
        }
        UmiCtestCoverageViewDestroy(view);
    }
    UmiCtestCoverageReviewDestroy(review);
    return 0;
}
