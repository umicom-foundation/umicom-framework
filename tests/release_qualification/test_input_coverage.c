/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_input_coverage.c
 * Purpose: Verify private-header and resource edits select owners and invalidate source observations.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
static UmiStatus ReadInput(void *context, const char *path, size_t limit, size_t *size, char digest[65])
{
    const char *changedPath = context;
    const char *value = changedPath != NULL && strcmp(changedPath, path) == 0 ? "after!" : "before";
    return QualificationRead((void *)value, path, limit, size, digest);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    bool shared = strcmp(name, "shared") == 0, unresolved = strcmp(name, "unresolved") == 0;
    REQUIRE(shared || unresolved || strcmp(name, "header") == 0 || strcmp(name, "resource") == 0);
    const char *input = strcmp(name, "header") == 0 ? "/repo/src/private.h" : "/repo/resources/defaults.ini";
    const char *relative = input + strlen("/repo/");
    Fixture text = {0};
    REQUIRE(FixtureStart(&text));
    REQUIRE(FixtureTarget(&text, "owner", NULL));
    REQUIRE(FixtureTarget(&text, "unrelated", NULL));
    REQUIRE(FixtureTest(&text, "owner.check", "owner", NULL, false));
    REQUIRE(FixtureTest(&text, "unrelated.check", "unrelated", NULL, false));
    REQUIRE(FixtureSource(&text, "/repo/unrelated.c", "unrelated"));
    REQUIRE(FixtureRow(&text, "source", input, "owner", unresolved ? "unresolved" : "present", "role=declared-input\n"));
    if (shared) {
        REQUIRE(FixtureTarget(&text, "second", NULL));
        REQUIRE(FixtureTest(&text, "second.check", "second", NULL, false));
        REQUIRE(FixtureRow(&text, "source", input, "second", "present", "role=declared-input\n"));
    }
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    UmiReleaseSourceObservation *before = NULL, *after = NULL;
    REQUIRE(FixtureParse(&text, &inventory) == UMI_STATUS_OK);
    REQUIRE(UmiReleaseImpactCreate(inventory, &relative, 1U, false, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactSummary selection;
    REQUIRE(UmiReleaseImpactDescribe(plan, &selection) == UMI_STATUS_OK);
    if (unresolved) REQUIRE((selection.fallbackReasons & UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE) != 0U);
    else {
        REQUIRE(selection.fallbackReasons == 0U && selection.targets == (shared ? 2U : 1U));
        REQUIRE(selection.tests == selection.targets);
        REQUIRE(PlanContains(plan, "owner", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
        REQUIRE(!PlanContains(plan, "unrelated", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
        if (shared) REQUIRE(PlanContains(plan, "second", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
    }
    UmiReleaseSourceOptions options = {.read = ReadInput,
        .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
    REQUIRE(UmiReleaseSourceObserve(inventory, &options, &before) == (unresolved ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
    if (unresolved) REQUIRE(before == NULL);
    else {
        REQUIRE(UmiReleaseSourceObservationCount(before) == 2U);
        options.context = (void *)input;
        REQUIRE(UmiReleaseSourceObserve(inventory, &options, &after) == UMI_STATUS_OK);
        UmiReleaseSourceComparison comparison;
        REQUIRE(UmiReleaseSourceObservationCompare(before, after, &comparison) == UMI_STATUS_OK);
        REQUIRE(!comparison.same && comparison.changedFiles == 1U && comparison.files == 2U);
    }
    UmiReleaseSourceObservationDestroy(before);
    UmiReleaseSourceObservationDestroy(after);
    UmiReleaseImpactDestroy(plan);
    UmiReleaseInventoryDestroy(inventory);
    FixtureDestroy(&text);
    return 0;
}
