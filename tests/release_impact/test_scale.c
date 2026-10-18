/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_scale.c
 * Purpose: Exercise a long dependency chain without recursion or fixed small target limits.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdio.h>
int main(void)
{
    Fixture f;
    REQUIRE(FixtureStart(&f));
    for (unsigned i = 0U; i < 4096U; ++i) {
        char name[32], dependency[32];
        (void)snprintf(name, sizeof(name), "target-%04u", i);
        (void)snprintf(dependency, sizeof(dependency), "target-%04u", i == 0U ? 0U : i - 1U);
        REQUIRE(FixtureTarget(&f, name, i == 0U ? NULL : dependency));
    }
    REQUIRE(FixtureSource(&f, "/repo/src/start.c", "target-0000"));
    REQUIRE(FixtureTest(&f, "test.last", "target-4095", NULL, false));
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *changes[] = {"src/start.c"};
    UmiReleaseImpactPlan *plan = NULL;
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, 1U, false, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary summary;
    REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.targets == 4096U && summary.tests == 1U && summary.fallbackReasons == 0U);
    REQUIRE(PlanContains(plan, "target-4095", UMI_RELEASE_IMPACT_DEPENDENT));
    REQUIRE(PlanContains(plan, "test.last", UMI_RELEASE_IMPACT_TEST_OWNER));
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
