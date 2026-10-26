/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_coverage.c
 *
 * PURPOSE:
 *   Keep host, source and skipped-result boundaries explicit in coverage reviews.
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

static UmiCtestCoverageRequirement Requirement(const char *name, const char *host)
{
    UmiCtestCoverageRequirement r = {0};
    (void)snprintf(r.name, sizeof r.name, "%s", name);
    (void)snprintf(r.host, sizeof r.host, "%s", host);
    (void)snprintf(r.configuration, sizeof r.configuration, "Debug");
    (void)snprintf(r.capability, sizeof r.capability, "Native display");
    return r;
}
static UmiCtestCoverageHost Host(const char *name, const UmiCtestReport *report)
{
    UmiCtestCoverageHost h = {0};
    (void)snprintf(h.host, sizeof h.host, "%s", name);
    (void)snprintf(h.configuration, sizeof h.configuration, "Debug");
    (void)snprintf(h.source_identity, sizeof h.source_identity, "source-A");
    h.report = report;
    return h;
}
static UmiCtestReport *PassedSkipName(void)
{
    const char xml[] = "<testsuite tests='1'><testcase name='c.skip' status='run' time='1'/></testsuite>";
    UmiCtestReport *report = NULL;
    OK(UmiCtestReportImport(xml, sizeof xml - 1U, &report));
    return report;
}
static int Large(void)
{
    const size_t count = 4096;
    UmiCtestReport *report = Many(count);
    UmiCtestCoverageHost host = Host("large-host", report);
    UmiCtestCoverageRequirement *requirements = calloc(count, sizeof *requirements);
    CHECK(requirements);
    for (size_t i = 0; i < count; ++i) {
        char name[64];
        (void)snprintf(name, sizeof name, "row-%06zu", i);
        requirements[i] = Requirement(name, "large-host");
    }
    UmiCtestCoverageReview *review = NULL;
    OK(UmiCtestCoverageReviewCreate("source-A", requirements, count, &host, 1, &review));
    UmiCtestCoverageSummary summary;
    OK(UmiCtestCoverageReviewRead(review, &summary));
    CHECK(summary.requirements == count && summary.outstanding == count);
    CHECK(summary.outcomes[UMI_CTEST_COVERAGE_FAILED] == count);
    CHECK(!strcmp(UmiCtestCoverageReviewAt(review, count - 1)->requirement.name, "row-004095"));
    CHECK(UmiCtestCoverageReviewAt(review, count) == NULL);
    free(requirements);
    UmiCtestReportDestroy(report);
    UmiCtestCoverageReviewDestroy(review);
    return 0;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"mixed", "passed", "missing-host", "missing-test", "stale",
        "configuration", "duplicate-host", "duplicate-requirement", "case-sensitive",
        "ownership", "empty", "null", "unterminated", "invalid-utf8", "capacity",
        "multiple-hosts", "skipped-other-host", "order", "failure-output", "large"};
    CHECK(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    CHECK(!strcmp(UmiCtestCoverageOutcomeText(UMI_CTEST_COVERAGE_OUTCOME_COUNT), "unknown"));
    CHECK(!strcmp(UmiCtestCoverageOutcomeText(UMI_CTEST_COVERAGE_MISSING_HOST), "missing host"));
    const char *name = argv[1];
    if (!strcmp(name, "large")) return Large();
    UmiCtestReport *report = Mixed(), *second = PassedSkipName();
    UmiCtestCoverageHost hosts[2] = {Host("windows", report), Host("linux", second)};
    const char *names[] = {"z.pass", "b.fail", "d.timeout", "c.skip", "e.disabled", "a.missing"};
    const UmiCtestCoverageOutcome outcomes[] = {UMI_CTEST_COVERAGE_PASSED, UMI_CTEST_COVERAGE_FAILED,
        UMI_CTEST_COVERAGE_TIMED_OUT, UMI_CTEST_COVERAGE_SKIPPED, UMI_CTEST_COVERAGE_DISABLED,
        UMI_CTEST_COVERAGE_NOT_RUN};
    UmiCtestCoverageRequirement requirements[6];
    for (size_t i = 0; i < 6; ++i) requirements[i] = Requirement(names[i], "windows");
    size_t required = 1, host_count = 1;
    UmiStatus expected = UMI_STATUS_OK;
    UmiCtestCoverageOutcome outcome = UMI_CTEST_COVERAGE_PASSED;
    const char *identity = "source-A";
    if (!strcmp(name, "mixed") || !strcmp(name, "order")) required = 6;
    if (!strcmp(name, "missing-host")) { host_count = 0; outcome = UMI_CTEST_COVERAGE_MISSING_HOST; }
    if (!strcmp(name, "missing-test")) {
        requirements[0] = Requirement("absent", "windows");
        outcome = UMI_CTEST_COVERAGE_MISSING_TEST;
    }
    if (!strcmp(name, "case-sensitive")) {
        requirements[0] = Requirement("Z.pass", "windows");
        outcome = UMI_CTEST_COVERAGE_MISSING_TEST;
    }
    if (!strcmp(name, "stale")) { identity = "source-B"; outcome = UMI_CTEST_COVERAGE_SOURCE_MISMATCH; }
    if (!strcmp(name, "configuration")) {
        strcpy(hosts[0].configuration, "Release");
        outcome = UMI_CTEST_COVERAGE_MISSING_HOST;
    }
    if (!strcmp(name, "duplicate-host")) {
        hosts[1] = hosts[0]; host_count = 2; expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (!strcmp(name, "duplicate-requirement")) {
        requirements[1] = requirements[0]; required = 2; expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (!strcmp(name, "unterminated")) {
        memset(requirements[0].host, 'x', sizeof requirements[0].host);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!strcmp(name, "invalid-utf8")) {
        requirements[0].capability[0] = (char)0x80;
        requirements[0].capability[1] = '\0';
    }
    if (!strcmp(name, "empty")) required = 0;
    if (!strcmp(name, "multiple-hosts")) {
        requirements[0] = Requirement("c.skip", "linux"); host_count = 2;
    }
    if (!strcmp(name, "skipped-other-host")) {
        requirements[0] = Requirement("c.skip", "windows");
        requirements[1] = Requirement("c.skip", "linux");
        required = 2; host_count = 2; outcome = UMI_CTEST_COVERAGE_SKIPPED;
    }
    if (!strcmp(name, "failure-output")) {
        hosts[0].report = NULL; expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiCtestCoverageReview *review = NULL;
    if (!strcmp(name, "null")) {
        CHECK(UmiCtestCoverageReviewCreate("source-A", requirements, 1, hosts, 1, NULL) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageReviewCreate(NULL, requirements, 1, hosts, 1, &review) ==
            UMI_STATUS_INVALID_ARGUMENT && !review);
        CHECK(UmiCtestCoverageReviewCreate("source-A", NULL, 1, hosts, 1, &review) ==
            UMI_STATUS_INVALID_ARGUMENT && !review);
        CHECK(UmiCtestCoverageReviewCreate("source-A", requirements, 1, NULL, 1, &review) ==
            UMI_STATUS_INVALID_ARGUMENT && !review);
        UmiCtestCoverageSummary sentinel, summary;
        memset(&sentinel, 0x5a, sizeof sentinel); summary = sentinel;
        CHECK(UmiCtestCoverageReviewRead(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&summary, &sentinel, sizeof summary) == 0);
        CHECK(UmiCtestCoverageRequirePassed(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestCoverageReviewAt(NULL, 0) == NULL);
        CHECK(UmiCtestCoverageReviewSourceIdentity(NULL) == NULL);
        goto done;
    }
    if (!strcmp(name, "capacity")) {
        CHECK(UmiCtestCoverageReviewCreate("source-A", requirements,
            UMI_CTEST_COVERAGE_MAX_REQUIREMENTS + 1U, hosts, 1, &review) ==
            UMI_STATUS_CAPACITY_EXCEEDED && !review);
        CHECK(UmiCtestCoverageReviewCreate("source-A", requirements, 1, hosts,
            UMI_CTEST_COVERAGE_MAX_HOSTS + 1U, &review) ==
            UMI_STATUS_CAPACITY_EXCEEDED && !review);
        goto done;
    }
    UmiStatus status = UmiCtestCoverageReviewCreate(identity, requirements, required,
        hosts, host_count, &review);
    if (!strcmp(name, "invalid-utf8")) { CHECK(status != UMI_STATUS_OK && !review); goto done; }
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK) { CHECK(!review); goto done; }
    if (!strcmp(name, "ownership")) {
        UmiCtestReportDestroy(report); report = NULL;
        memset(hosts, 0, sizeof hosts); memset(requirements, 0, sizeof requirements);
    }
    UmiCtestCoverageSummary summary;
    OK(UmiCtestCoverageReviewRead(review, &summary));
    CHECK(summary.requirements == required);
    CHECK(!strcmp(UmiCtestCoverageReviewSourceIdentity(review), identity));
    if (required == 0) {
        CHECK(summary.outstanding == 0 && UmiCtestCoverageReviewAt(review, 0) == NULL);
        CHECK(UmiCtestCoverageRequirePassed(review) == UMI_STATUS_INVALID_STATE);
    } else if (required == 6) {
        CHECK(summary.outstanding == 5);
        for (size_t i = 0; i < 6; ++i) {
            const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, i);
            CHECK(row && !strcmp(row->requirement.name, names[i]) && row->outcome == outcomes[i]);
            CHECK(row->has_observation && summary.outcomes[outcomes[i]] == 1);
        }
        CHECK(UmiCtestCoverageRequirePassed(review) == UMI_STATUS_INVALID_STATE);
    } else {
        const UmiCtestCoverageRow *row = UmiCtestCoverageReviewAt(review, 0);
        CHECK(row && row->outcome == outcome);
        CHECK(!strcmp(row->observed_source_identity,
            outcome == UMI_CTEST_COVERAGE_MISSING_HOST ? "" : "source-A"));
        CHECK(row->has_observation == (outcome < UMI_CTEST_COVERAGE_MISSING_HOST));
        if (!row->has_observation) {
            /* Explicit byte initialization makes the padding comparison portable. */
            UmiCtestReportSnapshot zero;
            memset(&zero, 0, sizeof zero);
            CHECK(memcmp(&row->observation, &zero, sizeof zero) == 0);
        }
        if (!strcmp(name, "ownership")) CHECK(!strcmp(row->requirement.host, "windows"));
        if (required == 2) {
            CHECK(UmiCtestCoverageReviewAt(review, 1)->outcome == UMI_CTEST_COVERAGE_PASSED);
            CHECK(summary.outstanding == 1);
        }
        CHECK(UmiCtestCoverageRequirePassed(review) ==
            (outcome == UMI_CTEST_COVERAGE_PASSED ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE));
    }
done:
    UmiCtestCoverageReviewDestroy(review);
    UmiCtestReportDestroy(report);
    UmiCtestReportDestroy(second);
    return 0;
}
