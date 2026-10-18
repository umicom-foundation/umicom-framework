/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_metadata.c
 * Purpose: Reject incomplete or malformed impact metadata without publishing a partial plan.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *detail = NULL;
    if (strcmp(mode, "flag") == 0) detail = "impact-version=1\ncomplete=yes\nimpact-end\n";
    if (strcmp(mode, "hex") == 0) detail = "impact-version=1\ncomplete=1\ndependency=6\nimpact-end\n";
    if (strcmp(mode, "unknown-key") == 0) detail = "impact-version=1\ncomplete=1\nexecute=1\nimpact-end\n";
    if (strcmp(mode, "truncated") == 0) detail = "impact-version=1\ncomplete=1\n";
    if (strcmp(mode, "duplicate-flag") == 0) detail = "impact-version=1\ncomplete=1\ncomplete=0\nimpact-end\n";
    if (strcmp(mode, "wrong-kind") == 0) detail = "impact-version=1\ncomplete=1\nalways=1\nimpact-end\n";
    REQUIRE(detail != NULL);
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureRow(&f, "target", "base", "/repo", "configured", detail));
    REQUIRE(FixtureSource(&f, "/repo/src/a.c", "base"));
    REQUIRE(FixtureTest(&f, "test.base", "base", NULL, false));
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    UmiReleaseImpactPlan *plan = NULL;
    const char *changes[] = {"src/a.c"};
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, 1U, false, &plan) == UMI_STATUS_PARSE_ERROR);
    REQUIRE(plan == NULL);
    UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
