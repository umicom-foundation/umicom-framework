/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_paths.c
 * Purpose: Cover path boundaries, renamed identities and changed-path validation.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "base", NULL));
    REQUIRE(FixtureTarget(&f, "moved", NULL));
    REQUIRE(FixtureSource(&f, "/repo/src/a.c", "base"));
    REQUIRE(FixtureSource(&f, "/repo/src/new.c", "moved"));
    REQUIRE(FixtureTest(&f, "test.base", "base", NULL, false));
    REQUIRE(FixtureTest(&f, "test.moved", "moved", NULL, false));
    if (strcmp(mode, "header") == 0)
        REQUIRE(FixtureRow(&f, "header", "include/base.h", "base", "declared",
            "impact-version=1\npath=2f7265706f2f696e636c7564652f626173652e68\nimpact-end\nsha256=synthetic"));
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    UmiReleaseImpactPlan *plan = NULL;
    const char *changes[3] = {"src/a.c", NULL, NULL};
    size_t count = 1U;
    bool invalid = strcmp(mode, "invalid") == 0;
    if (invalid) {
        const char *bad[] = {"", "../a.c", "src/../a.c", "./src/a.c", "/repo/src/a.c", "C:/repo/src/a.c", "src//a.c", "src/a.c/", "src/a\nc", NULL};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            REQUIRE(UmiReleaseImpactCreate(inventory, &bad[i], 1U, false, &plan) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(plan == NULL);
        }
        REQUIRE(UmiReleaseImpactCreate(inventory, changes, UMI_RELEASE_IMPACT_CHANGE_LIMIT + 1U, false, &plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else {
        bool rename = strcmp(mode, "rename") == 0;
        bool duplicate = strcmp(mode, "duplicate") == 0;
        bool boundary = strcmp(mode, "boundary") == 0;
        bool header = strcmp(mode, "header") == 0;
        bool separator = strcmp(mode, "separator") == 0;
        REQUIRE(rename || duplicate || boundary || header || separator);
        if (rename) { changes[1] = "src/new.c"; count = 2U; }
        if (duplicate) { changes[1] = changes[0]; changes[2] = "src\\a.c"; count = 3U; }
        if (boundary) changes[0] = "src/a.cpp";
        if (header) changes[0] = "include/base.h";
        if (separator) changes[0] = "src\\a.c";
        REQUIRE(UmiReleaseImpactCreate(inventory, changes, count, false, &plan) == UMI_STATUS_OK);
        UmiReleaseImpactSummary summary;
        REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.changedPaths == (rename ? 2U : 1U));
        if (boundary) REQUIRE((summary.fallbackReasons & UMI_RELEASE_IMPACT_UNMAPPED_PATH) != 0U);
        else REQUIRE(summary.fallbackReasons == 0U);
        REQUIRE(PlanContains(plan, "base", boundary ? UMI_RELEASE_IMPACT_FULL : UMI_RELEASE_IMPACT_CHANGED_SOURCE));
        REQUIRE(PlanContains(plan, "moved", 0U) == (rename || boundary));
    }
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
