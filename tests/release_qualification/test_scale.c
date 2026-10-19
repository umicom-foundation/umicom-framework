/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_scale.c
 * Purpose: Verify complete exact-name accounting beyond fixed UI-sized result lists.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    QualificationFixture f;
    bool longName = strcmp(argv[1], "long-name") == 0;
    REQUIRE(longName || strcmp(argv[1], "many-tests") == 0);
    REQUIRE(QualificationStart(&f, longName ? 1U : 5000U, false) == 0);
    if (longName) {
        UmiReleaseQualificationDestroy(f.run); f.run = NULL;
        UmiReleaseImpactDestroy(f.plan); f.plan = NULL;
        UmiReleaseInventoryDestroy(f.inventory); f.inventory = NULL;
        UmiReleaseSourceObservationDestroy(f.before); f.before = NULL;
        char name[8193]; memset(name, 'x', sizeof(name) - 1U); name[sizeof(name) - 1U] = '\0';
        REQUIRE(FixtureTest(&f.text, name, "library", NULL, false));
        REQUIRE(FixtureParse(&f.text, &f.inventory) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseImpactCreate(f.inventory, NULL, 0U, true, &f.plan) == UMI_STATUS_OK);
        UmiReleaseSourceOptions options = {.read = QualificationRead,
            .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
        REQUIRE(UmiReleaseSourceObserve(f.inventory, &options, &f.before) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationCreate(f.plan, f.before, f.execution, f.toolchain, &f.run) == UMI_STATUS_OK);
        REQUIRE(UmiReleaseQualificationGetIdentity(f.run, &f.identity) == UMI_STATUS_OK);
        REQUIRE(strcmp(UmiReleaseQualificationAt(f.run, 2U)->name, name) == 0);
    }
    REQUIRE(QualificationRecordAll(&f) == 0);
    REQUIRE(UmiReleaseQualificationClose(f.run, &f.identity, f.before, f.toolchain) == UMI_STATUS_OK);
    UmiReleaseQualificationSummary summary;
    REQUIRE(UmiReleaseQualificationDescribe(f.run, &summary) == UMI_STATUS_OK);
    REQUIRE(summary.tests == (longName ? 2U : 5000U) && summary.passed == summary.tests);
    REQUIRE(summary.readyForOwnerReview && summary.missing == 0U);
    QualificationDestroy(&f);
    return 0;
}
