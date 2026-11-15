/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_duplicate_failures.c
 *
 * PURPOSE:
 *   Verify duplicate evidence ownership, bounded growth and honest fingerprint-match reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/duplicate.h"
#include "umicom/codeguard/normalizer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static UmiStatus hash_status = UMI_STATUS_OK, insertion_status = UMI_STATUS_OUT_OF_MEMORY;
static unsigned hash_calls, insertion_calls, fail_insert;
static size_t allocation_bytes, old_bytes;
static int fail_allocation, move_allocation;
static uint64_t raw_value = 11U, normalized_value = 22U;
static size_t size_value = 9U;
static UmiStatus HashInput(const char *path, uint64_t *raw, uint64_t *normalized, size_t *size)
{
    (void)path;
    ++hash_calls;
    if (hash_status != UMI_STATUS_OK) return hash_status;
    *raw = raw_value;
    *normalized = normalized_value;
    *size = size_value;
    return UMI_STATUS_OK;
}
static void *Allocate(void *old, size_t bytes)
{
    allocation_bytes = bytes;
    if (fail_allocation) return NULL;
    if (move_allocation && old != NULL) {
        void *next = malloc(bytes);
        if (next == NULL) return NULL;
        memcpy(next, old, old_bytes);
        free(old);
        return next;
    }
    return realloc(old, bytes);
}
static UmiStatus Store(UmiCodeGuardResult *result, const UmiCodeGuardFinding *finding)
{
    ++insertion_calls;
    if (insertion_calls == fail_insert) return insertion_status;
    return umi_codeguard_result_add(result, finding);
}
/* Include actual collection code once. Fingerprints and allocator failures
 * are controlled; the result store remains the linked Framework service. */
#define umi_codeguard_hash_file HashInput
#define realloc Allocate
#define umi_codeguard_result_add Store
#include "../../src/codeguard/duplicate.c"
#undef umi_codeguard_result_add
#undef realloc
#undef umi_codeguard_hash_file

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiCodeGuardDuplicateSet *set = NULL;
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_duplicate_set_create(&set) == UMI_STATUS_OK);
    CHECK(umi_codeguard_result_create(8U, &result) == UMI_STATUS_OK);
    if (!strcmp(name, "arguments")) {
        CHECK(umi_codeguard_duplicate_set_add(NULL, "a.c") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_duplicate_set_add(set, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_duplicate_emit(NULL, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_duplicate_emit(set, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(hash_calls == 0U && umi_codeguard_duplicate_set_count(set) == 0U);
    } else if (!strcmp(name, "longpath") || !strcmp(name, "path-boundary")) {
        char path[1025];
        memset(path, 'a', sizeof path);
        path[!strcmp(name, "path-boundary") ? 1023U : 1024U] = '\0';
        UmiStatus expected = !strcmp(name, "path-boundary") ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(umi_codeguard_duplicate_set_add(set, path) == expected);
        CHECK(hash_calls == (expected == UMI_STATUS_OK ? 1U : 0U));
        if (expected == UMI_STATUS_OK) CHECK(!strcmp(set->items[0].path, path));
    } else if (!strcmp(name, "first-allocation")) {
        fail_allocation = 1;
        CHECK(umi_codeguard_duplicate_set_add(set, "a.c") == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(set->items == NULL && set->count == 0U && set->capacity == 0U);
    } else {
        CHECK(umi_codeguard_duplicate_set_add(set, "a.c") == UMI_STATUS_OK);
        UmiCodeGuardDuplicateFile *prior_items = set->items;
        size_t prior_capacity = set->capacity;
        if (!strcmp(name, "hash-failure") || !strcmp(name, "hash-capacity")) {
            hash_status = !strcmp(name, "hash-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_CAPACITY_EXCEEDED;
            CHECK(umi_codeguard_duplicate_set_add(set, "b.c") == hash_status);
            CHECK(set->count == 1U && set->items == prior_items && set->capacity == prior_capacity);
        } else if (!strcmp(name, "growth-failure") || !strcmp(name, "alias-growth")) {
            set->capacity = 1U;
            if (!strcmp(name, "growth-failure")) {
                fail_allocation = 1;
                CHECK(umi_codeguard_duplicate_set_add(set, "b.c") == UMI_STATUS_OUT_OF_MEMORY);
                CHECK(set->items == prior_items && set->count == 1U && set->capacity == 1U);
            } else {
                move_allocation = 1;
                old_bytes = sizeof(*set->items);
                CHECK(umi_codeguard_duplicate_set_add(set, set->items[0].path) == UMI_STATUS_OK);
                CHECK(set->count == 2U && set->capacity == 2U);
                CHECK(!strcmp(set->items[0].path, "a.c") && !strcmp(set->items[1].path, "a.c"));
            }
        } else if (!strcmp(name, "growth-boundary") || !strcmp(name, "full-capacity")) {
            const size_t maximum = SIZE_MAX / sizeof(*set->items);
            set->count = set->capacity = !strcmp(name, "full-capacity") ? maximum : maximum / 2U + 1U;
            fail_allocation = 1;
            unsigned previous_hashes = hash_calls;
            UmiStatus expected = !strcmp(name, "full-capacity") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OUT_OF_MEMORY;
            CHECK(umi_codeguard_duplicate_set_add(set, "b.c") == expected);
            if (expected == UMI_STATUS_OUT_OF_MEMORY) CHECK(allocation_bytes == maximum * sizeof(*set->items));
            else CHECK(hash_calls == previous_hashes);
            CHECK(set->items == prior_items);
            set->count = 1U;
            set->capacity = prior_capacity;
        } else if (!strcmp(name, "invalid-count") || !strcmp(name, "invalid-capacity") || !strcmp(name, "missing-storage")) {
            if (!strcmp(name, "invalid-count")) set->count = set->capacity + 1U;
            if (!strcmp(name, "invalid-capacity")) set->capacity = SIZE_MAX / sizeof(*set->items) + 1U;
            if (!strcmp(name, "missing-storage")) set->items = NULL;
            CHECK(umi_codeguard_duplicate_set_add(set, "b.c") == UMI_STATUS_INVALID_STATE);
            CHECK(umi_codeguard_duplicate_emit(set, result) == UMI_STATUS_INVALID_STATE);
            set->items = prior_items; set->count = 1U; set->capacity = prior_capacity;
        } else {
            if (!strcmp(name, "normalized")) raw_value = 33U;
            else if (!strcmp(name, "different")) { raw_value = 33U; normalized_value = 44U; }
            else if (!strcmp(name, "empty")) size_value = 0U;
            else if (!strcmp(name, "same-hash-different-size")) { size_value = 10U; normalized_value = 44U; }
            else if (strcmp(name, "raw") && strcmp(name, "first-emission") && strcmp(name, "second-emission") &&
                strcmp(name, "emission-capacity") && strcmp(name, "pair-order")) return 2;
            CHECK(umi_codeguard_duplicate_set_add(set, "b.c") == UMI_STATUS_OK);
            int three = !strcmp(name, "second-emission") || !strcmp(name, "pair-order");
            if (three) CHECK(umi_codeguard_duplicate_set_add(set, "c.c") == UMI_STATUS_OK);
            if (!strcmp(name, "first-emission")) fail_insert = 1U;
            if (!strcmp(name, "second-emission")) fail_insert = 2U;
            if (!strcmp(name, "emission-capacity")) { fail_insert = 1U; insertion_status = UMI_STATUS_CAPACITY_EXCEEDED; }
            CHECK(umi_codeguard_duplicate_emit(set, result) == (fail_insert ? insertion_status : UMI_STATUS_OK));
            size_t expected = fail_insert ? fail_insert - 1U : three ? 3U :
                (!strcmp(name, "different") || !strcmp(name, "empty") || !strcmp(name, "same-hash-different-size")) ? 0U : 1U;
            CHECK(umi_codeguard_result_count(result) == expected);
            CHECK(set->count == (three ? 3U : 2U));
            if (expected > 0U) {
                const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, 0U);
                CHECK(!strcmp(finding->path, "b.c") && strstr(finding->message, "matches a.c") != NULL);
                CHECK(strstr(finding->message, "Byte-identical") == NULL);
                CHECK(strstr(finding->remediation, "do not prove") != NULL);
                CHECK(!strcmp(finding->rule_id, !strcmp(name, "normalized") ? "CODEGUARD-DUP-002" : "CODEGUARD-DUP-001"));
                if (expected == 3U) {
                    CHECK(strstr(umi_codeguard_result_at(result, 1U)->message, "matches a.c") != NULL);
                    CHECK(strstr(umi_codeguard_result_at(result, 2U)->message, "matches b.c") != NULL);
                }
            }
        }
    }
    umi_codeguard_result_destroy(result);
    umi_codeguard_duplicate_set_destroy(set);
    return 0;
}
