/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_identity.c
 * Purpose: Reject cross-context and cross-attempt evidence while preserving owned selections.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>

int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *name = argv[1];
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 2U, strcmp(name, "no-work") == 0) == 0);
    if (strcmp(name, "no-work") == 0) {
        REQUIRE(f.run == NULL);
    } else if (strcmp(name, "ownership") == 0) {
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        UmiReleaseInventoryDestroy(f.inventory); f.inventory = NULL;
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        FixtureDestroy(&f.text);
        REQUIRE(UmiReleaseQualificationCount(f.run) == 3U);
        REQUIRE(strcmp(UmiReleaseQualificationAt(f.run, 0U)->name, "library") == 0);
        REQUIRE(strcmp(UmiReleaseQualificationAt(f.run, 2U)->name, "test.000001") == 0);
        REQUIRE(UmiReleaseQualificationAt(f.run, 3U) == NULL);
        REQUIRE(QualificationRecordAll(&f) == 0);
    } else if (strcmp(name, "attempt") == 0 || strcmp(name, "toolchain") == 0) {
        char other[65];
        REQUIRE(UmiSha256Buffer("other", 5U, other) == UMI_STATUS_OK);
        UmiReleaseQualification *second = NULL;
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before,
            strcmp(name, "attempt") == 0 ? other : f.execution,
            strcmp(name, "toolchain") == 0 ? other : f.toolchain, &second) == UMI_STATUS_OK);
        UmiReleaseQualificationIdentity identity;
        REQUIRE(UmiReleaseQualificationGetIdentity(second, &identity) == UMI_STATUS_OK);
        REQUIRE((strcmp(identity.plan, f.identity.plan) == 0) == (strcmp(name, "attempt") == 0));
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &identity, &result) == UMI_STATUS_INVALID_STATE);
        REQUIRE(UmiReleaseQualificationGetBuild(f.run, &result) == UMI_STATUS_NOT_FOUND);
        UmiReleaseQualificationDestroy(second);
    } else if (strcmp(name, "context") == 0) {
        char *generation = strstr(f.text.text, "0123456789abcdef0123456789abcdef");
        REQUIRE(generation != NULL); generation[0] = 'a';
        UmiReleaseInventory *other = NULL;
        UmiReleaseImpactPlan *plan = NULL;
        REQUIRE(FixtureParse(&f.text, &other) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseImpactCreate(other, NULL, 0U, true, &plan) == UMI_STATUS_OK);
        UmiReleaseQualification *rejected = NULL;
        REQUIRE(UmiReleaseQualificationCreate(plan, f.before, f.execution, f.toolchain, &rejected) == UMI_STATUS_INVALID_STATE);
        REQUIRE(rejected == NULL);
        UmiReleaseImpactDestroy(plan); UmiReleaseInventoryDestroy(other);
    } else if (strcmp(name, "invalid") == 0) {
        UmiReleaseQualification *sentinel = f.run;
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain, &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(sentinel == f.run);
        UmiReleaseQualification *empty = NULL;
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, "short", f.toolchain, &empty) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(empty == NULL);
        UmiReleaseQualificationIdentity wrong = f.identity; wrong.plan[64] = 'x';
        UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
        REQUIRE(UmiReleaseQualificationRecordBuild(f.run, &wrong, &result) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "source") == 0) {
        UmiReleaseSourceOptions options = {.read = QualificationRead, .context = "after!",
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        UmiReleaseSourceObservation *after = NULL;
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &after) == UMI_STATUS_OK);
        UmiReleaseQualification *second = NULL;
        REQUIRE(UmiReleaseQualificationCreate(f.plan, after, f.execution, f.toolchain, &second) == UMI_STATUS_OK);
        UmiReleaseQualificationIdentity identity;
        REQUIRE(UmiReleaseQualificationGetIdentity(second, &identity) == UMI_STATUS_OK);
        REQUIRE(strcmp(identity.plan, f.identity.plan) != 0);
        UmiReleaseQualificationDestroy(second); UmiReleaseSourceObservationDestroy(after);
    } else if (strcmp(name, "selection") == 0) {
        UmiReleaseImpactPlan *full = NULL;
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, true, &full) == UMI_STATUS_OK);
        UmiReleaseQualification *second = NULL;
        REQUIRE(UmiReleaseQualificationCreate(full, f.before, f.execution, f.toolchain, &second) == UMI_STATUS_OK);
        UmiReleaseQualificationIdentity identity;
        REQUIRE(UmiReleaseQualificationGetIdentity(second, &identity) == UMI_STATUS_OK);
        REQUIRE(strcmp(identity.plan, f.identity.plan) != 0);
        UmiReleaseQualificationDestroy(second); UmiReleaseImpactDestroy(full);
    } else REQUIRE(false);
    REQUIRE(UmiReleaseQualificationCount(NULL) == 0U);
    REQUIRE(UmiReleaseQualificationAt(NULL, 0U) == NULL);
    QualificationDestroy(&f);
    return 0;
}
