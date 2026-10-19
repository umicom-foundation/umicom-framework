/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/fixture.c
 * Purpose: Create deterministic owned inputs and explicit execution evidence.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
UmiStatus QualificationRead(void *context, const char *path, size_t limit, size_t *outSize, char digest[65])
{
    (void)path;
    const char *bytes = context == NULL ? "before" : (const char *)context;
    size_t length = strlen(bytes);
    if (length > limit) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiSha256Buffer(bytes, length, digest);
    if (status == UMI_STATUS_OK) *outSize = length;
    return status;
}
int QualificationStart(QualificationFixture *f, size_t tests, bool noChanges)
{
    memset(f, 0, sizeof(*f));
    REQUIRE(FixtureStart(&f->text));
    REQUIRE(FixtureTarget(&f->text, "library", NULL));
    REQUIRE(FixtureSource(&f->text, "/repo/library.c", "library"));
    for (size_t i = 0U; i < tests; ++i) {
        char name[64];
        (void)snprintf(name, sizeof(name), "test.%06zu", i);
        REQUIRE(FixtureTest(&f->text, name, "library", NULL, false));
    }
    REQUIRE(FixtureParse(&f->text, &f->inventory) == UMI_STATUS_OK);
    const char *changes[] = {"library.c"};
    REQUIRE(UmiReleaseImpactCreate(f->inventory, changes, noChanges ? 0U : 1U, false, &f->plan) == UMI_STATUS_OK);
    UmiReleaseSourceOptions options = {.read = QualificationRead,
        .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT, .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT};
    REQUIRE(UmiReleaseSourceObserve(f->inventory, &options, &f->before) == UMI_STATUS_OK);
    REQUIRE(UmiSha256Buffer("attempt", 7U, f->execution) == UMI_STATUS_OK);
    REQUIRE(UmiSha256Buffer("compiler-and-config", 19U, f->toolchain) == UMI_STATUS_OK);
    UmiStatus status = UmiReleaseQualificationCreate(f->plan, f->before, f->execution, f->toolchain, &f->run);
    REQUIRE(status == (noChanges ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
    if (!noChanges) REQUIRE(UmiReleaseQualificationGetIdentity(f->run, &f->identity) == UMI_STATUS_OK);
    return 0;
}
void QualificationDestroy(QualificationFixture *f)
{
    UmiReleaseQualificationDestroy(f->run);
    UmiReleaseSourceObservationDestroy(f->before);
    UmiReleaseImpactDestroy(f->plan);
    UmiReleaseInventoryDestroy(f->inventory);
    FixtureDestroy(&f->text);
    memset(f, 0, sizeof(*f));
}
UmiReleaseQualificationResult QualificationResult(UmiReleaseQualificationOutcome outcome)
{
    UmiReleaseQualificationResult result = {.outcome = outcome, .status = UMI_STATUS_OK,
        .exitCode = outcome == UMI_QUALIFICATION_FAILED ? 1 : 0, .durationMs = 123U, .exitCodeKnown = true};
    (void)UmiSha256Buffer("captured log", 12U, result.logSha256);
    return result;
}
int QualificationRecordAll(QualificationFixture *f)
{
    UmiReleaseQualificationResult result = QualificationResult(UMI_QUALIFICATION_PASSED);
    REQUIRE(UmiReleaseQualificationRecordBuild(f->run, &f->identity, &result) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UmiReleaseQualificationCount(f->run); ++i) {
        const UmiReleaseImpactItem *item = UmiReleaseQualificationAt(f->run, i);
        if (item->kind == UMI_RELEASE_INVENTORY_TEST)
            REQUIRE(UmiReleaseQualificationRecordTest(f->run, &f->identity, item->name, &result) == UMI_STATUS_OK);
    }
    return 0;
}
