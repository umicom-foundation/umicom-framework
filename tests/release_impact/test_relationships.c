/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_relationships.c
 * Purpose: Exercise exact CTest relationship closure, prerequisites and conservative fallback.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdlib.h>
#include <string.h>

static int Scale(void)
{
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "runner", NULL));
    REQUIRE(FixtureTarget(&f, "other", NULL));
    REQUIRE(FixtureSource(&f, "/repo/src/other.c", "other"));
    /* Only one case starts selected. A long related-test chain must terminate
     * without recursion or adding the same registration to the work queue twice. */
    for (unsigned i = 0U; i < 4096U; ++i) {
        char name[32], next[32];
        (void)snprintf(name, sizeof(name), "test.%04u", i);
        (void)snprintf(next, sizeof(next), "test.%04u", (i + 1U) % 4096U);
        const char *related[] = {next};
        REQUIRE(FixtureRelatedTest(&f, name, "runner", related, 1U, i == 0U));
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *changes[] = {"src/other.c"};
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, 1U, false, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary summary;
    REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.fallbackReasons == 0U && summary.tests == 4096U && summary.targets == 2U);
    REQUIRE(PlanContains(plan, "runner", UMI_RELEASE_IMPACT_TEST_PREREQUISITE));
    REQUIRE(PlanContains(plan, "test.4095", UMI_RELEASE_IMPACT_TEST_DEPENDENCY));
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "scale") == 0) return Scale();
    const char *cases[] = {"chain", "reverse", "cycle", "overlap", "duplicate", "self",
        "missing", "unowned", "always", "no-changes", "full", "legacy-key", "wrong-kind"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0) known = true;
    REQUIRE(known);
    bool reverse = strcmp(mode, "reverse") == 0;
    bool cycle = strcmp(mode, "cycle") == 0;
    bool overlap = strcmp(mode, "overlap") == 0;
    bool duplicate = strcmp(mode, "duplicate") == 0;
    bool self = strcmp(mode, "self") == 0;
    bool missing = strcmp(mode, "missing") == 0;
    bool unowned = strcmp(mode, "unowned") == 0;
    bool always = strcmp(mode, "always") == 0;
    bool empty = strcmp(mode, "no-changes") == 0;
    bool full = strcmp(mode, "full") == 0;
    bool legacy = strcmp(mode, "legacy-key") == 0;
    bool wrong = strcmp(mode, "wrong-kind") == 0;
    Fixture f;
    REQUIRE(FixtureStart(&f));
    const char *targets[] = {"a", "b", "c", "d", "other"};
    for (size_t i = 0U; i < sizeof(targets) / sizeof(targets[0]); ++i)
        REQUIRE(FixtureTarget(&f, targets[i], NULL));
    REQUIRE(FixtureSource(&f, "/repo/src/a.c", "a"));
    REQUIRE(FixtureSource(&f, "/repo/src/c.c", "c"));
    REQUIRE(FixtureSource(&f, "/repo/src/other.c", "other"));
    const char *aRelated[] = {"test.b", duplicate ? "test.b" : "test.a"};
    const char *bRelated[] = {"test.c"};
    const char *cRelated[] = {missing ? "absent" : "test.a"};
    const char *dRelated[] = {"test.c"};
    if (legacy)
        REQUIRE(FixtureRow(&f, "test", "test.a", "/repo", "registered",
            "impact-version=1\ncomplete=1\ntarget=61\ntest=746573742e62\nimpact-end\n"));
    else REQUIRE(FixtureRelatedTest(&f, "test.a", "a", aRelated,
        duplicate || self ? 2U : 1U, always));
    REQUIRE(FixtureRelatedTest(&f, "test.b", unowned ? NULL : "b", bRelated, 1U, false));
    REQUIRE(FixtureRelatedTest(&f, "test.c", "c", cRelated, cycle || missing ? 1U : 0U, false));
    REQUIRE(FixtureRelatedTest(&f, "test.d", "d", dRelated, overlap ? 1U : 0U, false));
    REQUIRE(FixtureTest(&f, "test.other", "other", NULL, false));
    if (wrong)
        REQUIRE(FixtureRow(&f, "target", "bad", "/repo", "configured",
            "impact-version=2\ncomplete=1\ntest=746573742e62\nimpact-end\n"));
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *changes[] = {reverse ? "src/c.c" : always ? "src/other.c" : "src/a.c"};
    UmiStatus status = UmiReleaseImpactCreate(inventory, changes, empty ? 0U : 1U, full, &plan);
    if (legacy || wrong) {
        REQUIRE(status == UMI_STATUS_PARSE_ERROR && plan == NULL);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        UmiReleaseImpactSummary summary;
        REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
        bool broad = missing || unowned || full;
        REQUIRE((summary.fallbackReasons != 0U) == broad);
        if (empty) {
            REQUIRE(summary.noWork && summary.targets == 0U && summary.tests == 0U);
        } else {
            REQUIRE(!summary.noWork && !summary.missingTestCoverage);
            REQUIRE(summary.tests == (broad ? 5U : overlap || always ? 4U : 3U));
            REQUIRE(summary.targets == summary.tests);
            REQUIRE(PlanContains(plan, "test.a", 0U));
            REQUIRE(PlanContains(plan, "test.b", broad ? UMI_RELEASE_IMPACT_FULL : UMI_RELEASE_IMPACT_TEST_DEPENDENCY));
            REQUIRE(PlanContains(plan, "test.c", 0U));
            REQUIRE(PlanContains(plan, "test.d", 0U) == (broad || overlap));
            REQUIRE(PlanContains(plan, "test.other", 0U) == (broad || always));
            REQUIRE(PlanContains(plan, "b", broad ? UMI_RELEASE_IMPACT_FULL : UMI_RELEASE_IMPACT_TEST_PREREQUISITE));
            if (!broad) {
                char expression[256]; size_t required = 0U;
                REQUIRE(UmiReleaseImpactTestExpression(plan, expression, sizeof(expression), &required) == UMI_STATUS_OK);
                REQUIRE(strstr(expression, "test\\.b") != NULL && strstr(expression, "test\\.c") != NULL);
                REQUIRE(strstr(expression, "test\\.other") != NULL || !always);
            }
        }
    }
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
