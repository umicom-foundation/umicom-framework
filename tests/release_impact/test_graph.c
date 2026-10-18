/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_graph.c
 * Purpose: Exercise dependency direction, cycles and test build prerequisites.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    bool cycle = strcmp(mode, "cycle") == 0;
    bool reverse = strcmp(mode, "reverse") == 0;
    bool shared = strcmp(mode, "shared-source") == 0;
    bool helper = strcmp(mode, "prerequisite") == 0;
    bool always = strcmp(mode, "always") == 0;
    REQUIRE(cycle || reverse || shared || helper || always || strcmp(mode, "chain") == 0);
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "base", cycle ? "app" : NULL));
    REQUIRE(FixtureTarget(&f, "app", "base"));
    REQUIRE(FixtureTarget(&f, "app-test", "app"));
    REQUIRE(FixtureTarget(&f, "other", NULL));
    REQUIRE(FixtureTarget(&f, "helper", NULL));
    REQUIRE(FixtureSource(&f, "/repo/src/base.c", "base"));
    REQUIRE(FixtureSource(&f, "/repo/src/app.c", "app"));
    if (shared) REQUIRE(FixtureSource(&f, "/repo/src/base.c", "other"));
    REQUIRE(FixtureTest(&f, "test.app", "app-test", helper ? "helper" : NULL, false));
    REQUIRE(FixtureTest(&f, "test.other", "other", NULL, false));
    if (always) REQUIRE(FixtureTest(&f, "test.always", "helper", NULL, true));
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    UmiReleaseImpactPlan *plan = NULL;
    const char *changes[] = {reverse ? "src/app.c" : "src/base.c"};
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, 1U, false, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary summary;
    REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.fallbackReasons == 0U && !summary.noWork && !summary.missingTestCoverage);
    REQUIRE(PlanContains(plan, "app", reverse ? UMI_RELEASE_IMPACT_CHANGED_SOURCE : UMI_RELEASE_IMPACT_DEPENDENT));
    REQUIRE(PlanContains(plan, "app-test", UMI_RELEASE_IMPACT_DEPENDENT));
    REQUIRE(PlanContains(plan, "test.app", UMI_RELEASE_IMPACT_TEST_OWNER));
    REQUIRE(PlanContains(plan, "base", 0U) == !reverse);
    REQUIRE(PlanContains(plan, "other", 0U) == shared);
    REQUIRE(PlanContains(plan, "test.other", 0U) == shared);
    REQUIRE(PlanContains(plan, "helper", UMI_RELEASE_IMPACT_TEST_PREREQUISITE) == (helper || always));
    if (always) REQUIRE(PlanContains(plan, "test.always", UMI_RELEASE_IMPACT_ALWAYS));
    /* Results own their display names, so source metadata may be released. */
    UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    REQUIRE(PlanContains(plan, "test.app", UMI_RELEASE_IMPACT_TEST_OWNER));
    REQUIRE(UmiReleaseImpactAt(plan, UmiReleaseImpactCount(plan)) == NULL);
    UmiReleaseImpactDestroy(plan);
    return 0;
}
