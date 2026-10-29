/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_observed_retries.c
 *
 * PURPOSE:
 *   Combine immutable source observations and retry identities without losing either.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/distribution/runtime/observed_impact.h"
#include <string.h>

typedef struct Reader { bool edited; size_t calls; } Reader;
static UmiStatus Read(void *context, const char *name, size_t maximumBytes,
    size_t *outSize, char outSha256[65])
{
    Reader *reader = context;
    ++reader->calls;
    const char *bytes = reader->edited && strcmp(name, "/repo/base.c") == 0 ? "B" : "A";
    if (maximumBytes < 1U) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiSha256Buffer(bytes, 1U, outSha256);
    if (status == UMI_STATUS_OK) *outSize = 1U;
    return status;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *modes[] = {"unchanged-retry", "changed-and-retry", "related",
        "missing-baseline", "unknown", "empty", "duplicate", "null-list",
        "invalid-name", "occupied-output", "null-summary", "ownership"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (strcmp(mode, modes[i]) == 0) known = true;
    REQUIRE(known);
    bool edited = strcmp(mode, "changed-and-retry") == 0;
    bool related = strcmp(mode, "related") == 0;
    bool missing = strcmp(mode, "missing-baseline") == 0;
    bool unknown = strcmp(mode, "unknown") == 0;
    bool empty = strcmp(mode, "empty") == 0;
    Fixture f;
    REQUIRE(FixtureStart(&f));
    REQUIRE(FixtureTarget(&f, "base", NULL));
    REQUIRE(FixtureTarget(&f, "other", NULL));
    REQUIRE(FixtureTarget(&f, "setup", NULL));
    REQUIRE(FixtureSource(&f, "/repo/base.c", "base"));
    REQUIRE(FixtureSource(&f, "/repo/other.c", "other"));
    REQUIRE(FixtureTest(&f, "base.test", "base", NULL, false));
    const char *relationship[] = {"setup.test"};
    REQUIRE(FixtureRelatedTest(&f, "other.test", "other", relationship, related ? 1U : 0U, false));
    REQUIRE(FixtureTest(&f, "setup.test", "setup", NULL, false));
    UmiReleaseInventory *inventory = NULL;
    UmiReleaseSourceObservation *baseline = NULL, *current = NULL;
    UmiReleaseImpactPlan *plan = NULL, *original = NULL;
    REQUIRE(FixtureParse(&f, &inventory) == UMI_STATUS_OK);
    Reader reader = {0};
    UmiReleaseSourceOptions options = {.maximumFileBytes = 16U, .maximumTotalBytes = 32U,
        .read = Read, .context = &reader};
    REQUIRE(UmiReleaseSourceObserve(inventory, &options, &baseline) == UMI_STATUS_OK);
    reader.edited = edited;
    REQUIRE(UmiReleaseSourceObserve(inventory, &options, &current) == UMI_STATUS_OK);
    const char *names[] = {unknown ? "absent" : "other.test", "other.test"};
    if (strcmp(mode, "invalid-name") == 0) names[0] = "";
    if (strcmp(mode, "occupied-output") == 0) {
        REQUIRE(UmiReleaseImpactCreate(inventory, NULL, 0U, false, &plan) == UMI_STATUS_OK);
        original = plan;
    }
    size_t reads = reader.calls;
    UmiObservedImpactSummary observed = {71U, 72U, 73U, true};
    UmiStatus status = UmiReleaseImpactFromObservationsWithTests(inventory,
        missing ? NULL : baseline, current,
        strcmp(mode, "null-list") == 0 ? NULL : names,
        empty ? 0U : strcmp(mode, "duplicate") == 0 ? 2U : 1U, &plan,
        strcmp(mode, "null-summary") == 0 ? NULL : &observed);
    REQUIRE(reader.calls == reads);
    bool invalid = strcmp(mode, "invalid-name") == 0 || strcmp(mode, "null-list") == 0 ||
        strcmp(mode, "null-summary") == 0 || original != NULL;
    if (invalid) {
        REQUIRE(status == UMI_STATUS_INVALID_ARGUMENT && plan == original);
        REQUIRE(observed.observedFiles == 71U && observed.changedFiles == 72U &&
            observed.fallbackReasons == 73U && observed.changesKnown);
    } else {
        REQUIRE(status == UMI_STATUS_OK);
        REQUIRE(observed.observedFiles == 2U && observed.changedFiles == (edited ? 1U : 0U));
        REQUIRE(observed.changesKnown == !missing);
        REQUIRE(observed.fallbackReasons == (missing ? UMI_OBSERVED_IMPACT_BASELINE_MISSING : 0U));
        UmiReleaseImpactSummary summary;
        REQUIRE(UmiReleaseImpactDescribe(plan, &summary) == UMI_STATUS_OK);
        size_t expected = missing || unknown ? 3U : empty ? 0U : edited || related ? 2U : 1U;
        REQUIRE(summary.targets == expected && summary.tests == expected);
        REQUIRE(summary.noWork == empty);
        if (!empty && !unknown) REQUIRE(PlanContains(plan, "other.test", UMI_RELEASE_IMPACT_REQUIRED_TEST));
        if (related) REQUIRE(PlanContains(plan, "setup.test", UMI_RELEASE_IMPACT_TEST_DEPENDENCY));
        if (edited) REQUIRE(PlanContains(plan, "base", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
        if (strcmp(mode, "ownership") == 0) {
            UmiReleaseSourceObservationDestroy(current); current = NULL;
            UmiReleaseSourceObservationDestroy(baseline); baseline = NULL;
            UmiReleaseInventoryDestroy(inventory); inventory = NULL;
            REQUIRE(PlanContains(plan, "other.test", UMI_RELEASE_IMPACT_REQUIRED_TEST));
        }
    }
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(current); UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseInventoryDestroy(inventory); FixtureDestroy(&f);
    return 0;
}
