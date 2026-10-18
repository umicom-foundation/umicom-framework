/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_fallback.c
 * Purpose: Keep incomplete ownership and unknown changes visible through full selection.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    bool old = strcmp(mode, "old-inventory") == 0;
    bool unknown = strcmp(mode, "unknown-path") == 0;
    bool build = strcmp(mode, "configuration") == 0;
    bool unowned = strcmp(mode, "unowned-test") == 0;
    bool unresolved = strcmp(mode, "unresolved-source") == 0;
    bool dependency = strcmp(mode, "missing-dependency") == 0;
    bool full = strcmp(mode, "explicit-full") == 0;
    bool empty = strcmp(mode, "no-changes") == 0;
    bool noTests = strcmp(mode, "no-tests") == 0;
    REQUIRE(old || unknown || build || unowned || unresolved || dependency || full || empty || noTests);
    Fixture f;
    REQUIRE(FixtureStart(&f));
    if (old) REQUIRE(FixtureRow(&f, "target", "base", "/repo", "configured", "TYPE=STATIC_LIBRARY\n"));
    else REQUIRE(FixtureTarget(&f, "base", dependency ? "not-captured" : NULL));
    REQUIRE(FixtureTarget(&f, "unrelated", NULL));
    REQUIRE(FixtureSource(&f, "/repo/src/base.c", "base"));
    if (unresolved) REQUIRE(FixtureRow(&f, "source", "$<CONFIG>/generated.c", "base", "unresolved", "raw expression"));
    if (!noTests) {
        if (unowned) REQUIRE(FixtureRow(&f, "test", "test.base", "/repo", "registered", "no ownership"));
        else REQUIRE(FixtureTest(&f, "test.base", "base", NULL, false));
        REQUIRE(FixtureTest(&f, "test.unrelated", "unrelated", NULL, false));
    }
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *changes[] = {unknown ? "src/new.c" : build ? "cmake/toolchain.cmake" : "src/base.c"};
    UmiReleaseImpactPlan *plan = NULL;
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, empty || full ? 0U : 1U, full, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary summary;
    REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
    if (empty) {
        REQUIRE(summary.noWork && summary.targets == 0U && summary.tests == 0U);
        REQUIRE(!summary.missingTestCoverage && summary.fallbackReasons == 0U);
    } else if (noTests) {
        REQUIRE(summary.missingTestCoverage && summary.tests == 0U && summary.targets == 1U);
    } else {
        REQUIRE(summary.targets == 2U && summary.tests == 2U);
        REQUIRE(PlanContains(plan, "unrelated", UMI_RELEASE_IMPACT_FULL));
        uint32_t expected = old || dependency ? UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH :
            unknown ? UMI_RELEASE_IMPACT_UNMAPPED_PATH : build ? UMI_RELEASE_IMPACT_BUILD_CONFIGURATION :
            unowned ? UMI_RELEASE_IMPACT_UNOWNED_TEST : unresolved ? UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE :
            UMI_RELEASE_IMPACT_REQUESTED_FULL;
        REQUIRE((summary.fallbackReasons & expected) != 0U);
    }
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
