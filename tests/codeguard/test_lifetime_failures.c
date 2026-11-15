/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_lifetime_failures.c
 *
 * PURPOSE:
 *   Verify checked lifetime evidence, bounded tracker ownership and finding insertion failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/codeguard/lifetime.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static size_t insertion_count, fail_at;
static UmiStatus insertion_failure = UMI_STATUS_OUT_OF_MEMORY;
static UmiStatus InsertFinding(UmiCodeGuardResult *result, const UmiCodeGuardFinding *finding)
{
    ++insertion_count;
    if (insertion_count == fail_at) return insertion_failure;
    return umi_codeguard_result_add(result, finding);
}
/* Include actual lifetime analysis once; only storage failure is injected.
 * Result storage remains the shared implementation linked below. */
#define umi_codeguard_result_add InsertFinding
#include "../../src/codeguard/lifetime.c"
#undef umi_codeguard_result_add

static int SameTracker(const UmiCodeGuardLifetimeTracker *a,
    const UmiCodeGuardLifetimeTracker *b)
{
    return a->count == b->count &&
        memcmp(a->freed_names, b->freed_names, sizeof a->freed_names) == 0 &&
        memcmp(a->freed_lines, b->freed_lines, sizeof a->freed_lines) == 0;
}
static UmiStatus Line(UmiCodeGuardLifetimeTracker *tracker, const char *text,
    UmiCodeGuardResult *result)
{
    return umi_codeguard_lifetime_scan_checked(tracker, "example.c", 42U, text, result);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiCodeGuardLifetimeTracker tracker;
    umi_codeguard_lifetime_init(&tracker);
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(8U, &result) == UMI_STATUS_OK);
    if (!strcmp(name, "arguments")) {
        CHECK(Line(NULL, "free(p);", result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Line(&tracker, NULL, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Line(&tracker, "free(p);", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_lifetime_scan_checked(&tracker, NULL, 1U, "free(p);", result) ==
            UMI_STATUS_INVALID_ARGUMENT);
        CHECK(tracker.count == 0U && insertion_count == 0U);
    } else if (!strcmp(name, "invalid-count")) {
        tracker.count = 65U;
        UmiCodeGuardLifetimeTracker prior = tracker;
        CHECK(Line(&tracker, "free(p);", result) == UMI_STATUS_INVALID_STATE);
        CHECK(SameTracker(&tracker, &prior) && insertion_count == 0U);
    } else if (!strcmp(name, "capacity") || !strcmp(name, "reuse")) {
        for (size_t i = 0U; i < 64U; ++i) {
            char line[64];
            (void)snprintf(line, sizeof line, "free(pointer_%zu);", i);
            CHECK(Line(&tracker, line, result) == UMI_STATUS_OK);
        }
        CHECK(tracker.count == 64U);
        UmiCodeGuardLifetimeTracker full = tracker;
        CHECK(Line(&tracker, "free(extra);", result) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(SameTracker(&tracker, &full) && insertion_count == 0U);
        if (!strcmp(name, "reuse")) {
            CHECK(Line(&tracker, "pointer_17 = replacement;", result) == UMI_STATUS_OK);
            CHECK(Line(&tracker, "free(extra);", result) == UMI_STATUS_OK);
            CHECK(tracker.count == 64U && !strcmp(tracker.freed_names[17], "extra"));
            CHECK(Line(&tracker, "extra->field = 1;", result) == UMI_STATUS_OK);
            CHECK(umi_codeguard_result_count(result) == 1U);
        }
    } else if (!strcmp(name, "realloc") || !strcmp(name, "realloc-failure")) {
        if (!strcmp(name, "realloc-failure")) fail_at = 1U;
        UmiCodeGuardLifetimeTracker prior = tracker;
        CHECK(Line(&tracker, "owner = realloc(owner, 64);", result) ==
            (fail_at ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK));
        CHECK(SameTracker(&tracker, &prior));
        CHECK(umi_codeguard_result_count(result) == (fail_at ? 0U : 1U));
        if (!fail_at) CHECK(!strcmp(umi_codeguard_result_at(result, 0U)->rule_id, "CODEGUARD-C-MEM-005"));
    } else if (!strcmp(name, "partial-line")) {
        CHECK(Line(&tracker, "free(first);", result) == UMI_STATUS_OK);
        CHECK(Line(&tracker, "free(second);", result) == UMI_STATUS_OK);
        UmiCodeGuardLifetimeTracker prior = tracker;
        fail_at = 2U;
        CHECK(Line(&tracker, "first->x = second[0];", result) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(SameTracker(&tracker, &prior));
        CHECK(umi_codeguard_result_count(result) == 1U);
        CHECK(!strcmp(umi_codeguard_result_at(result, 0U)->rule_id, "CODEGUARD-C-MEM-003"));
    } else if (!strcmp(name, "assignment-rollback")) {
        CHECK(Line(&tracker, "free(owner);", result) == UMI_STATUS_OK);
        UmiCodeGuardLifetimeTracker prior = tracker;
        fail_at = 1U;
        CHECK(Line(&tracker, "owner = realloc(owner, 64);", result) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(SameTracker(&tracker, &prior));
    } else if (!strcmp(name, "legacy")) {
        umi_codeguard_lifetime_scan(&tracker, "example.c", 1U, "free(p);", result);
        umi_codeguard_lifetime_scan(&tracker, "example.c", 2U, "p->x = 1;", result);
        CHECK(umi_codeguard_result_count(result) == 1U && tracker.freed_names[0][0] == '\0');
    } else {
        CHECK(Line(&tracker, "free(owner);", result) == UMI_STATUS_OK);
        UmiCodeGuardLifetimeTracker prior = tracker;
        int double_free = !strncmp(name, "double-free", 11U);
        int failure = strstr(name, "failure") != NULL || !strcmp(name, "capacity-status");
        if (!double_free && strcmp(name, "use-after-free") &&
            strcmp(name, "use-after-free-failure") && strcmp(name, "capacity-status")) return 2;
        if (failure) fail_at = 1U;
        if (!strcmp(name, "capacity-status")) insertion_failure = UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(Line(&tracker, double_free ? "free(owner);" : "owner->state = 1;", result) ==
            (failure ? insertion_failure : UMI_STATUS_OK));
        CHECK(umi_codeguard_result_count(result) == (failure ? 0U : 1U));
        if (failure || double_free) CHECK(SameTracker(&tracker, &prior));
        else CHECK(tracker.freed_names[0][0] == '\0');
        if (!failure) {
            const UmiCodeGuardFinding *item = umi_codeguard_result_at(result, 0U);
            CHECK(item != NULL && item->line == 42U && item->severity == UMI_CODEGUARD_HIGH);
            CHECK(!strcmp(item->rule_id, double_free ? "CODEGUARD-C-MEM-004" : "CODEGUARD-C-MEM-003"));
        }
    }
    umi_codeguard_result_destroy(result);
    return 0;
}
