/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_required_tests.c
 *
 * PURPOSE:
 *   Keep explicit retries, fixture groups and build owners in affected selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include <stdlib.h>
#include <string.h>

static int Scale(void)
{
    Fixture f;
    REQUIRE(FixtureStart(&f) && FixtureTarget(&f, "runner", NULL));
    for (unsigned i = 0U; i < 4096U; ++i) {
        char name[32], next[32];
        (void)snprintf(name, sizeof(name), "case.%04u", i);
        (void)snprintf(next, sizeof(next), "case.%04u", (i + 1U) % 4096U);
        const char *related[] = {next};
        REQUIRE(FixtureRelatedTest(&f, name, "runner", related, 1U, false));
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    const char *required[] = {"case.2048"};
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseImpactCreateWithTests(inventory, NULL, 0U,
        required, 1U, false, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary summary;
    REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.tests == 4096U && summary.targets == 1U && !summary.noWork);
    REQUIRE(summary.fallbackReasons == 0U && summary.changedPaths == 0U);
    REQUIRE(PlanContains(plan, "case.2048", UMI_RELEASE_IMPACT_REQUIRED_TEST));
    REQUIRE(PlanContains(plan, "case.0000", UMI_RELEASE_IMPACT_TEST_DEPENDENCY));
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "scale") == 0) return Scale();
    const char *modes[] = {"single", "duplicate", "related", "reverse", "self", "always",
        "source", "union", "unknown", "case-sensitive", "empty", "full", "no-tests",
        "incomplete", "malformed", "unicode", "literal", "null-list", "null-name",
        "empty-name", "count-limit", "name-limit", "total-limit", "null-inventory", "null-output",
        "occupied-output", "ownership", "legacy"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (strcmp(mode, modes[i]) == 0) known = true;
    REQUIRE(known);
    bool source = strcmp(mode, "source") == 0 || strcmp(mode, "union") == 0;
    bool empty = strcmp(mode, "empty") == 0;
    bool noTests = strcmp(mode, "no-tests") == 0;
    bool full = strcmp(mode, "full") == 0;
    bool relation = strcmp(mode, "related") == 0 || strcmp(mode, "reverse") == 0;
    bool self = strcmp(mode, "self") == 0;
    bool always = strcmp(mode, "always") == 0;
    bool incomplete = strcmp(mode, "incomplete") == 0;
    bool malformed = strcmp(mode, "malformed") == 0;
    const char *name = strcmp(mode, "unicode") == 0 ? "café.check" :
        strcmp(mode, "literal") == 0 ? "suite.[one]+(two)" : "suite.test";
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "lib", NULL));
    REQUIRE(FixtureTarget(&f, "runner", "lib"));
    REQUIRE(FixtureTarget(&f, "setup", NULL));
    REQUIRE(FixtureTarget(&f, "other", NULL));
    if (incomplete || malformed)
        REQUIRE(FixtureRow(&f, "target", "unused", "/repo", "configured",
            malformed ? "impact-version=2\ncomplete=invalid\nimpact-end\n" :
            "impact-version=2\ncomplete=0\nimpact-end\n"));
    else REQUIRE(FixtureTarget(&f, "unused", NULL));
    REQUIRE(FixtureSource(&f, "/repo/lib.c", "lib"));
    if (!noTests) {
        const char *related[] = {self ? name : "suite.setup"};
        REQUIRE(FixtureRelatedTest(&f, name, "runner", related, relation || self ? 1U : 0U, false));
        REQUIRE(FixtureTest(&f, "suite.sibling", "runner", NULL, false));
        REQUIRE(FixtureTest(&f, "suite.setup", "setup", NULL, false));
        REQUIRE(FixtureTest(&f, "other.test", "other", NULL, false));
        REQUIRE(FixtureTest(&f, "always.test", "unused", NULL, always));
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseImpactPlan *plan = NULL, *occupied = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *required[2] = {name, name};
    size_t count = strcmp(mode, "duplicate") == 0 ? 2U : empty ? 0U : 1U;
    if (strcmp(mode, "reverse") == 0) required[0] = "suite.setup";
    if (strcmp(mode, "union") == 0) required[0] = "other.test";
    if (strcmp(mode, "unknown") == 0) required[0] = "missing.test";
    if (strcmp(mode, "case-sensitive") == 0) required[0] = "SUITE.TEST";
    if (strcmp(mode, "null-name") == 0) required[0] = NULL;
    if (strcmp(mode, "empty-name") == 0) required[0] = "";
    if (strcmp(mode, "count-limit") == 0) count = UMI_RELEASE_IMPACT_CHANGE_LIMIT + 1U;
    char *longName = NULL;
    if (strcmp(mode, "name-limit") == 0) {
        longName = malloc((size_t)UMI_RELEASE_INVENTORY_FIELD_LIMIT + 2U);
        REQUIRE(longName != NULL);
        memset(longName, 'a', (size_t)UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U);
        longName[UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U] = '\0';
        required[0] = longName;
    }
    const char **requests = required;
    const char *manyNames[257];
    if (strcmp(mode, "total-limit") == 0) {
        longName = malloc((size_t)UMI_RELEASE_INVENTORY_FIELD_LIMIT + 1U);
        REQUIRE(longName != NULL);
        memset(longName, 'a', UMI_RELEASE_INVENTORY_FIELD_LIMIT);
        longName[UMI_RELEASE_INVENTORY_FIELD_LIMIT] = '\0';
        for (size_t i = 0U; i < 257U; ++i) manyNames[i] = longName;
        requests = manyNames; count = 257U;
    }
    if (strcmp(mode, "occupied-output") == 0) {
        REQUIRE(UmiReleaseImpactCreate(inventory, NULL, 0U, false, &plan) == UMI_STATUS_OK);
        occupied = plan;
    }
    const char *changes[] = {"lib.c"};
    UmiStatus status = strcmp(mode, "legacy") == 0 ?
        UmiReleaseImpactCreate(inventory, NULL, 0U, false, &plan) :
        UmiReleaseImpactCreateWithTests(strcmp(mode, "null-inventory") == 0 ? NULL : inventory,
            changes, source ? 1U : 0U,
            strcmp(mode, "null-list") == 0 ? NULL : requests, count, full,
            strcmp(mode, "null-output") == 0 ? NULL : &plan);
    free(longName);
    bool invalid = strcmp(mode, "null-list") == 0 || strcmp(mode, "null-name") == 0 ||
        strcmp(mode, "empty-name") == 0 || strcmp(mode, "null-inventory") == 0 ||
        strcmp(mode, "null-output") == 0 || occupied != NULL;
    bool capacity = strcmp(mode, "count-limit") == 0 || strcmp(mode, "name-limit") == 0 ||
        strcmp(mode, "total-limit") == 0;
    if (invalid || capacity || malformed) {
        REQUIRE(status == (invalid ? UMI_STATUS_INVALID_ARGUMENT :
            capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
        REQUIRE(plan == occupied);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        UmiReleaseImpactSummary summary;
        REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
        bool unknown = strcmp(mode, "unknown") == 0 || strcmp(mode, "case-sensitive") == 0 || noTests;
        bool broad = unknown || full || incomplete;
        REQUIRE((summary.fallbackReasons != 0U) == broad);
        REQUIRE(summary.changedPaths == (source ? 1U : 0U));
        if (empty || strcmp(mode, "legacy") == 0) {
            REQUIRE(summary.noWork && summary.tests == 0U && summary.targets == 0U);
        } else if (broad) {
            REQUIRE(!summary.noWork && summary.targets == 5U && summary.tests == (noTests ? 0U : 5U));
            REQUIRE(summary.missingTestCoverage == noTests);
            if (unknown) REQUIRE((summary.fallbackReasons & UMI_RELEASE_IMPACT_UNMAPPED_TEST) != 0U);
        } else {
            size_t expected = source ? (strcmp(mode, "union") == 0 ? 3U : 2U) :
                relation || always ? 2U : 1U;
            REQUIRE(!summary.noWork && !summary.missingTestCoverage);
            REQUIRE(summary.targets == expected && summary.tests == expected);
            REQUIRE(PlanContains(plan, required[0], UMI_RELEASE_IMPACT_REQUIRED_TEST));
            if (relation) REQUIRE(PlanContains(plan, "setup", UMI_RELEASE_IMPACT_TEST_PREREQUISITE));
            if (!source) REQUIRE(!PlanContains(plan, "suite.sibling", 0U));
            REQUIRE(!PlanContains(plan, "lib", 0U) || source);
            char expression[512]; size_t size = 0U;
            REQUIRE(UmiReleaseImpactTestExpression(plan, expression, sizeof(expression), &size) == UMI_STATUS_OK);
            if (strcmp(mode, "literal") == 0) REQUIRE(strstr(expression, "\\[one\\]") != NULL);
        }
        if (strcmp(mode, "ownership") == 0) {
            UmiReleaseInventoryDestroy(inventory); inventory = NULL;
            FixtureDestroy(&f);
            REQUIRE(PlanContains(plan, "suite.test", UMI_RELEASE_IMPACT_REQUIRED_TEST));
            REQUIRE(UmiReleaseImpactInventoryFingerprint(plan) != NULL);
        }
    }
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
