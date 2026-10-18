/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_expression.c
 * Purpose: Check exact CTest regex boundaries, capacity refusal and empty-selection handling.
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
    bool full = strcmp(mode, "full") == 0;
    bool empty = strcmp(mode, "empty") == 0;
    bool control = strcmp(mode, "control") == 0;
    bool capacity = strcmp(mode, "capacity") == 0;
    bool large = strcmp(mode, "large") == 0;
    REQUIRE(full || empty || control || capacity || large || strcmp(mode, "exact") == 0);
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "base", NULL));
    REQUIRE(FixtureSource(&f, "/repo/a.c", "base"));
    if (!empty) {
        REQUIRE(FixtureTest(&f, control ? "test\ncontrol" : "alpha.[x](a)+?", "base", NULL, false));
        REQUIRE(FixtureTest(&f, "beta|$^*{}\\", "base", NULL, false));
    }
    if (large) {
        char *name = malloc(25001U);
        REQUIRE(name != NULL);
        memset(name, 'a', 25000U); name[25000] = '\0';
        REQUIRE(FixtureTest(&f, name, "base", NULL, false));
        free(name);
    }
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    const char *changes[] = {"a.c"};
    UmiReleaseImpactPlan *plan = NULL;
    REQUIRE(UmiReleaseImpactCreate(inventory, changes, 1U, full, &plan) == UMI_STATUS_OK);
    char result[256] = "unchanged";
    size_t required = 777U;
    UmiStatus status = UmiReleaseImpactTestExpression(plan, result, capacity ? 2U : sizeof(result), &required);
    if (full || empty || control || large || capacity) {
        REQUIRE(status == (capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_UNAVAILABLE));
        REQUIRE(strcmp(result, "unchanged") == 0 && required == 777U);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        REQUIRE(strcmp(result, "^(alpha\\.\\[x\\]\\(a\\)\\+\\?|beta\\|\\$\\^\\*\\{\\}\\\\)$") == 0);
        REQUIRE(required == strlen(result) + 1U);
        size_t measured = 0U;
        REQUIRE(UmiReleaseImpactTestExpression(plan, NULL, 0U, &measured) == UMI_STATUS_OK && measured == required);
    }
    UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
