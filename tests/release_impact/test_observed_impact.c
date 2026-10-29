/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_impact/test_observed_impact.c
 *
 * PURPOSE:
 *   Verify observed edits retain dependency closure and conservative fallbacks.
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
#include <stdlib.h>
#include <string.h>

typedef struct Reader {
    const char *changed;
    bool all, longer, empty;
    size_t calls;
} Reader;

static UmiStatus Read(void *context, const char *path, size_t maximumBytes,
    size_t *outSize, char outSha256[65])
{
    Reader *reader = context;
    ++reader->calls;
    bool changed = reader->all || (reader->changed != NULL &&
        strcmp(path, reader->changed) == 0);
    const char *bytes = changed ? (reader->empty ? "" :
        reader->longer ? "new contents" : "bbbb") : "aaaa";
    size_t size = strlen(bytes);
    if (size > maximumBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiSha256Buffer(bytes, size, outSha256);
    if (status == UMI_STATUS_OK) *outSize = size;
    return status;
}

/* Change only a synthetic context field. Inventory parsing remains the public
 * parser, so tests exercise its identity contract rather than internal structs. */
static bool Replace(Fixture *fixture, const char *from, const char *to)
{
    char *at = strstr(fixture->text, from);
    if (at == NULL) return false;
    size_t first = (size_t)(at - fixture->text), a = strlen(from), b = strlen(to);
    size_t size = fixture->used - a + b;
    char *next = malloc(size + 1U);
    if (next == NULL) return false;
    memcpy(next, fixture->text, first);
    memcpy(next + first, to, b);
    memcpy(next + first + b, at + a, fixture->used - first - a + 1U);
    free(fixture->text);
    fixture->text = next;
    fixture->used = size;
    fixture->capacity = size + 1U;
    return true;
}

static bool Is(const char *value, const char *expected)
{
    return strcmp(value, expected) == 0;
}

int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    const char *mode = argv[1];
    const char *const modes[] = {
        "same", "changed-hash", "changed-size", "header", "duplicate-source",
        "related-tests", "missing-baseline", "inventory-changed", "current-stale",
        "outside", "boundary", "case-difference", "configuration-change",
        "unowned-source", "incomplete-graph", "malformed-graph", "null-inventory",
        "null-current", "null-plan", "null-summary", "occupied-plan", "ownership",
        "all-changed", "empty-file", "windows-separators", "unicode", "scale",
        "independent"
    };
    bool known = false;
    for (size_t i = 0U; i < sizeof(modes) / sizeof(modes[0]); ++i)
        if (Is(mode, modes[i])) known = true;
    REQUIRE(known);

    Fixture fixture = {0};
    REQUIRE(FixtureStart(&fixture));
    REQUIRE(FixtureTarget(&fixture, "base", NULL));
    REQUIRE(FixtureTarget(&fixture, "consumer", "base"));
    REQUIRE(FixtureTarget(&fixture, "other", NULL));
    const char *source = Is(mode, "outside") ? "/external/base.c" :
        Is(mode, "boundary") ? "/repository/base.c" :
        Is(mode, "case-difference") ? "/Repo/base.c" :
        Is(mode, "configuration-change") ? "/repo/CMakeLists.txt" :
        Is(mode, "windows-separators") ? "C:\\repo\\base.c" :
        Is(mode, "unicode") ? "/repo/caf\xc3\xa9.c" : "/repo/base.c";
    const char *second = Is(mode, "windows-separators") ? "C:\\repo\\other.c" : "/repo/other.c";
    REQUIRE(FixtureSource(&fixture, source, Is(mode, "unowned-source") ? "absent" : "base"));
    REQUIRE(FixtureSource(&fixture, second, "other"));
    REQUIRE(FixtureTest(&fixture, "consumer.check", "consumer", NULL, false));
    REQUIRE(FixtureTest(&fixture, "other.check", "other", NULL, false));
    if (Is(mode, "header"))
        REQUIRE(FixtureRow(&fixture, "header", "include/api.h", "base", "declared",
            "impact-version=1\npath=2f7265706f2f696e636c7564652f6170692e68\nimpact-end\n"));
    if (Is(mode, "duplicate-source")) REQUIRE(FixtureSource(&fixture, source, "consumer"));
    if (Is(mode, "related-tests")) {
        const char *related[] = {"consumer.check"};
        REQUIRE(FixtureTarget(&fixture, "helper", NULL));
        REQUIRE(FixtureRelatedTest(&fixture, "setup", "helper", related, 1U, false));
    }
    if (Is(mode, "incomplete-graph"))
        REQUIRE(FixtureRow(&fixture, "target", "legacy", "/repo", "configured", "TYPE=STATIC_LIBRARY"));
    if (Is(mode, "malformed-graph"))
        REQUIRE(FixtureRow(&fixture, "target", "broken", "/repo", "configured",
            "impact-version=1\ncomplete=1\nunknown=1\nimpact-end\n"));
    if (Is(mode, "windows-separators"))
        REQUIRE(Replace(&fixture, "\t2f7265706f\t", "\t433a2f7265706f\t"));
    if (Is(mode, "scale")) {
        for (size_t i = 0U; i < 2048U; ++i) {
            char name[64];
            (void)snprintf(name, sizeof(name), "/repo/input-%04zu.c", i);
            REQUIRE(FixtureSource(&fixture, name, "base"));
        }
    }
    UmiReleaseInventory *inventory = NULL, *otherInventory = NULL;
    UmiReleaseSourceObservation *baseline = NULL, *current = NULL;
    REQUIRE(FixtureParse(&fixture, &inventory) == UMI_STATUS_OK);
    Reader reader = {0};
    UmiReleaseSourceOptions options = {
        .maximumFileBytes = 1024U, .maximumTotalBytes = 32768U,
        .read = Read, .context = &reader
    };
    REQUIRE(UmiReleaseSourceObserve(inventory, &options, &baseline) == UMI_STATUS_OK);
    if (!Is(mode, "same"))
        reader.changed = Is(mode, "header") ? "/repo/include/api.h" :
            Is(mode, "windows-separators") ? "C:/repo/base.c" :
            Is(mode, "independent") ? "/repo/other.c" : source;
    reader.all = Is(mode, "all-changed") || Is(mode, "scale");
    reader.longer = Is(mode, "changed-size");
    reader.empty = Is(mode, "empty-file");
    if (Is(mode, "inventory-changed") || Is(mode, "current-stale")) {
        REQUIRE(Replace(&fixture, "0123456789abcdef0123456789abcdef",
            "1123456789abcdef0123456789abcdef"));
        REQUIRE(FixtureParse(&fixture, &otherInventory) == UMI_STATUS_OK);
    }
    const UmiReleaseInventory *currentInventory =
        Is(mode, "inventory-changed") ? otherInventory : inventory;
    REQUIRE(UmiReleaseSourceObserve(currentInventory, &options, &current) == UMI_STATUS_OK);
    size_t calls = reader.calls;
    UmiReleaseImpactPlan *plan = NULL;
    if (Is(mode, "occupied-plan"))
        REQUIRE(UmiReleaseImpactCreate(inventory, NULL, 0U, true, &plan) == UMI_STATUS_OK);
    UmiReleaseImpactPlan *original = plan;
    UmiObservedImpactSummary summary = {91U, 92U, 93U, true};
    UmiStatus expected = Is(mode, "current-stale") ? UMI_STATUS_INVALID_STATE :
        Is(mode, "malformed-graph") ? UMI_STATUS_PARSE_ERROR :
        strncmp(mode, "null-", 5U) == 0 || Is(mode, "occupied-plan") ?
        UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    UmiStatus status = UmiReleaseImpactFromObservations(
        Is(mode, "null-inventory") ? NULL : Is(mode, "current-stale") ?
        otherInventory : currentInventory,
        Is(mode, "missing-baseline") ? NULL : baseline,
        Is(mode, "null-current") ? NULL : current,
        Is(mode, "null-plan") ? NULL : &plan,
        Is(mode, "null-summary") ? NULL : &summary);
    REQUIRE(status == expected);
    REQUIRE(reader.calls == calls); /* Planning never reopens source files. */
    if (status != UMI_STATUS_OK) {
        REQUIRE(plan == original);
        REQUIRE(summary.observedFiles == 91U && summary.changedFiles == 92U &&
            summary.fallbackReasons == 93U && summary.changesKnown);
    } else {
        UmiReleaseImpactSummary selection;
        REQUIRE(UmiReleaseImpactDescribe(plan, &selection) == UMI_STATUS_OK);
        uint32_t fallback = Is(mode, "missing-baseline") ? UMI_OBSERVED_IMPACT_BASELINE_MISSING :
            Is(mode, "inventory-changed") ? UMI_OBSERVED_IMPACT_INVENTORY_CHANGED :
            Is(mode, "outside") || Is(mode, "boundary") || Is(mode, "case-difference") ?
            UMI_OBSERVED_IMPACT_OUTSIDE_ROOT : 0U;
        bool changesKnown = !Is(mode, "missing-baseline") && !Is(mode, "inventory-changed");
        size_t files = Is(mode, "header") ? 3U : Is(mode, "scale") ? 2050U : 2U;
        size_t changed = !changesKnown || Is(mode, "same") ? 0U :
            reader.all ? files : 1U;
        REQUIRE(summary.observedFiles == files && summary.changedFiles == changed);
        REQUIRE(summary.changesKnown == changesKnown && summary.fallbackReasons == fallback);
        bool full = fallback != 0U || Is(mode, "configuration-change") ||
            Is(mode, "unowned-source") || Is(mode, "incomplete-graph");
        if (full) {
            REQUIRE(!selection.noWork && selection.fallbackReasons != 0U);
            REQUIRE(PlanContains(plan, "base", UMI_RELEASE_IMPACT_FULL));
            REQUIRE(PlanContains(plan, "consumer.check", UMI_RELEASE_IMPACT_FULL));
            REQUIRE(PlanContains(plan, "other.check", UMI_RELEASE_IMPACT_FULL));
        } else if (Is(mode, "same")) {
            REQUIRE(selection.noWork && selection.targets == 0U && selection.tests == 0U);
            REQUIRE(UmiReleaseImpactCount(plan) == 0U);
        } else if (Is(mode, "independent")) {
            REQUIRE(selection.targets == 1U && selection.tests == 1U);
            REQUIRE(PlanContains(plan, "other", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
            REQUIRE(!PlanContains(plan, "base", 0U));
            REQUIRE(!PlanContains(plan, "consumer.check", 0U));
        } else {
            REQUIRE(!selection.noWork && selection.fallbackReasons == 0U);
            REQUIRE(PlanContains(plan, "base", UMI_RELEASE_IMPACT_CHANGED_SOURCE));
            REQUIRE(PlanContains(plan, "consumer", UMI_RELEASE_IMPACT_DEPENDENT));
            REQUIRE(PlanContains(plan, "consumer.check", UMI_RELEASE_IMPACT_TEST_OWNER));
            REQUIRE(PlanContains(plan, "other.check", 0U) == reader.all);
            if (Is(mode, "related-tests")) {
                REQUIRE(PlanContains(plan, "setup", UMI_RELEASE_IMPACT_TEST_DEPENDENCY));
                REQUIRE(PlanContains(plan, "helper", UMI_RELEASE_IMPACT_TEST_PREREQUISITE));
            }
        }
        if (Is(mode, "ownership")) {
            UmiReleaseSourceObservationDestroy(baseline); baseline = NULL;
            UmiReleaseSourceObservationDestroy(current); current = NULL;
            UmiReleaseInventoryDestroy(inventory); inventory = NULL;
            FixtureDestroy(&fixture);
            REQUIRE(PlanContains(plan, "consumer.check", UMI_RELEASE_IMPACT_TEST_OWNER));
            REQUIRE(UmiReleaseImpactDescribe(plan, &selection) == UMI_STATUS_OK);
            REQUIRE(selection.targets == 2U && selection.tests == 1U);
        }
    }
    UmiReleaseImpactDestroy(plan);
    UmiReleaseSourceObservationDestroy(baseline);
    UmiReleaseSourceObservationDestroy(current);
    UmiReleaseInventoryDestroy(inventory);
    UmiReleaseInventoryDestroy(otherInventory);
    FixtureDestroy(&fixture);
    return 0;
}
