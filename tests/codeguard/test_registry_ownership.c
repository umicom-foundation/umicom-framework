/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_registry_ownership.c
 *
 * PURPOSE:
 *   Verify checked rule registry growth, batch refusal and descriptor self-appends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/rule_registry.h"
#include "umicom/codeguard/pattern_scan.h"
#include "umicom/codeguard/result.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static int refuse_allocation;
static size_t allocations, requested_bytes;
static void *RegistryAllocate(size_t bytes)
{
    ++allocations;
    requested_bytes = bytes;
    return refuse_allocation ? NULL : malloc(bytes);
}
/* Only descriptor-array allocation is controlled. Registry creation, matching,
 * storage and builtin rule catalogues use their actual implementations. */
#define malloc RegistryAllocate
#include "../../src/codeguard/rule_registry.c"
#undef malloc
static UmiCodeGuardRule Rule(const char *id, const char *needle)
{
    UmiCodeGuardRule rule = {id, "", UMI_CODEGUARD_MEDIUM,
        UMI_CODEGUARD_CATEGORY_MEMORY, 100U, UMI_CODEGUARD_MATCH_CODE,
        needle, "Review a match", "Inspect the source"};
    return rule;
}
static int Verify(UmiCodeGuardRuleRegistry *registry, size_t count)
{
    CHECK(umi_codeguard_rule_registry_count(registry) == count);
    CHECK(umi_codeguard_rule_registry_at(registry, count) == NULL);
    for (size_t i = 0U; i < count; ++i) {
        const UmiCodeGuardRule *rule = umi_codeguard_rule_registry_at(registry, i);
        CHECK(rule != NULL && rule->needle != NULL);
    }
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiCodeGuardRuleRegistry *registry = NULL;
    CHECK(umi_codeguard_rule_registry_create(&registry) == UMI_STATUS_OK);
    UmiCodeGuardRule rules[128];
    char borrowed_needle[] = "token";
    for (size_t i = 0U; i < 128U; ++i) rules[i] = Rule("TEST-RULE", "token");
    rules[1].rule_id = "TEST-SECOND";
    rules[2].rule_id = "TEST-THIRD";
    const size_t maximum = SIZE_MAX / sizeof(UmiCodeGuardRule);
    if (!strcmp(name, "arguments")) {
        CHECK(umi_codeguard_rule_registry_add(NULL, rules, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_rule_registry_add(registry, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_rule_registry_count(NULL) == 0U);
        CHECK(umi_codeguard_rule_registry_at(NULL, 0U) == NULL);
        CHECK(Verify(registry, 0U) == 0 && allocations == 0U);
    } else if (!strcmp(name, "empty")) {
        CHECK(umi_codeguard_rule_registry_add(registry, NULL, 0U) == UMI_STATUS_OK);
        CHECK(Verify(registry, 0U) == 0 && allocations == 0U);
    } else if (!strcmp(name, "first-allocation")) {
        refuse_allocation = 1;
        CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(Verify(registry, 0U) == 0 && registry->items == NULL && registry->capacity == 0U);
        CHECK(allocations == 1U);
    } else if (!strcmp(name, "excess-count")) {
        CHECK(umi_codeguard_rule_registry_add(registry, rules, maximum + 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(Verify(registry, 0U) == 0 && allocations == 0U);
    } else if (!strcmp(name, "null-first") || !strcmp(name, "null-later")) {
        rules[!strcmp(name, "null-first") ? 0U : 2U].needle = NULL;
        CHECK(umi_codeguard_rule_registry_add(registry, rules, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Verify(registry, 0U) == 0 && allocations == 0U);
    } else if (!strcmp(name, "ordinary") || !strcmp(name, "descriptor-copy")) {
        CHECK(umi_codeguard_rule_registry_add(registry, rules, 3U) == UMI_STATUS_OK);
        if (!strcmp(name, "descriptor-copy"))
            rules[0].rule_id = "CHANGED-INPUT-DESCRIPTOR";
        CHECK(Verify(registry, 3U) == 0);
        CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, 0U)->rule_id, "TEST-RULE"));
        CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, 1U)->rule_id, "TEST-SECOND"));
        CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, 2U)->rule_id, "TEST-THIRD"));
    } else if (!strcmp(name, "borrowed-strings")) {
        rules[0].needle = borrowed_needle;
        CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_OK);
        CHECK(umi_codeguard_rule_registry_at(registry, 0U)->needle == borrowed_needle);
        /* The caller owns these bytes and keeps them alive and unchanged. */
    } else if (!strcmp(name, "optional-text") || !strcmp(name, "empty-needle")) {
        rules[0].rule_id = rules[0].cwe = rules[0].message = rules[0].remediation = NULL;
        if (!strcmp(name, "empty-needle")) rules[0].needle = "";
        CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_OK);
        UmiCodeGuardResult *result = NULL;
        CHECK(umi_codeguard_result_create(1U, &result) == UMI_STATUS_OK);
        CHECK(umi_codeguard_pattern_scan_line(registry, "x.c", 1U, "token", "token", result) == UMI_STATUS_OK);
        CHECK(umi_codeguard_result_count(result) == 1U);
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, 0U);
        CHECK(finding != NULL && finding->column == 1U && finding->rule_id[0] == '\0');
        umi_codeguard_result_destroy(result);
    } else if (!strcmp(name, "builtins")) {
        CHECK(umi_codeguard_rule_registry_add_builtin(registry) == UMI_STATUS_OK);
        CHECK(umi_codeguard_rule_registry_count(registry) > 0U);
        CHECK(Verify(registry, umi_codeguard_rule_registry_count(registry)) == 0);
    } else {
        size_t initial = !strcmp(name, "alias-without-growth") ? 3U : 128U;
        CHECK(umi_codeguard_rule_registry_add(registry, rules, initial) == UMI_STATUS_OK);
        UmiCodeGuardRule *storage = registry->items;
        size_t capacity = registry->capacity, before_allocations = allocations;
        if (!strcmp(name, "alias-growth") || !strcmp(name, "alias-without-growth")) {
            const UmiCodeGuardRule *borrow = umi_codeguard_rule_registry_at(registry, 1U);
            CHECK(umi_codeguard_rule_registry_add(registry, borrow, 2U) == UMI_STATUS_OK);
            CHECK(Verify(registry, initial + 2U) == 0);
            CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, initial)->rule_id, "TEST-SECOND"));
            CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, initial + 1U)->rule_id, "TEST-THIRD"));
            CHECK(allocations == before_allocations + (initial == 128U ? 1U : 0U));
        } else if (!strcmp(name, "growth-failure") || !strcmp(name, "alias-failure")) {
            refuse_allocation = 1;
            const UmiCodeGuardRule *source = !strcmp(name, "alias-failure") ?
                umi_codeguard_rule_registry_at(registry, 1U) : rules;
            CHECK(umi_codeguard_rule_registry_add(registry, source, 1U) == UMI_STATUS_OUT_OF_MEMORY);
            CHECK(registry->items == storage && registry->capacity == capacity);
            CHECK(Verify(registry, initial) == 0);
            CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, 1U)->rule_id, "TEST-SECOND"));
        } else if (!strcmp(name, "null-batch-rollback")) {
            rules[2].needle = NULL;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 3U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(registry->items == storage && registry->capacity == capacity);
            CHECK(Verify(registry, initial) == 0 && allocations == before_allocations);
        } else if (!strcmp(name, "count-overflow")) {
            CHECK(umi_codeguard_rule_registry_add(registry, rules, maximum) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(registry->items == storage && Verify(registry, initial) == 0);
            CHECK(allocations == before_allocations);
        } else if (!strcmp(name, "invalid-count")) {
            registry->count = capacity + 1U;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_INVALID_STATE);
            CHECK(allocations == before_allocations);
            registry->count = initial;
        } else if (!strcmp(name, "invalid-capacity")) {
            registry->capacity = maximum + 1U;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_INVALID_STATE);
            CHECK(allocations == before_allocations);
            registry->capacity = capacity;
        } else if (!strcmp(name, "missing-storage")) {
            registry->items = NULL;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_INVALID_STATE);
            CHECK(allocations == before_allocations);
            registry->items = storage;
        } else if (!strcmp(name, "growth-boundary")) {
            /* Simulate representable counters, refusing allocation before any
             * read of the large old range. No huge allocation is performed. */
            registry->count = registry->capacity = maximum - 1U;
            refuse_allocation = 1;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_OUT_OF_MEMORY);
            CHECK(requested_bytes == maximum * sizeof(UmiCodeGuardRule));
            CHECK(allocations == before_allocations + 1U && registry->items == storage);
            CHECK(registry->count == maximum - 1U && registry->capacity == maximum - 1U);
            registry->count = initial; registry->capacity = capacity;
        } else if (!strcmp(name, "full-capacity")) {
            registry->count = registry->capacity = maximum;
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(allocations == before_allocations && registry->items == storage);
            registry->count = initial; registry->capacity = capacity;
        } else if (!strcmp(name, "empty-populated")) {
            CHECK(umi_codeguard_rule_registry_add(registry, NULL, 0U) == UMI_STATUS_OK);
            CHECK(Verify(registry, initial) == 0 && registry->items == storage);
            CHECK(allocations == before_allocations);
        } else if (!strcmp(name, "growth-success")) {
            CHECK(umi_codeguard_rule_registry_add(registry, rules, 1U) == UMI_STATUS_OK);
            CHECK(Verify(registry, initial + 1U) == 0);
            CHECK(registry->capacity >= initial + 1U);
            CHECK(!strcmp(umi_codeguard_rule_registry_at(registry, 1U)->rule_id, "TEST-SECOND"));
        } else return 2;
    }
    umi_codeguard_rule_registry_destroy(registry);
    return 0;
}
