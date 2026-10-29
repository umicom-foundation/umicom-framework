/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_shelf_faults.c
 *
 * PURPOSE:
 *   Exercise allocation refusal and generation exhaustion in the real shelf implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include "shelf_fixture.h"

/* The narrow harness compiles the real ownership code with a controllable
 * allocator. Production APIs do not acquire testing hooks or mutable counters. */
static size_t allocations, fail_at;
static void *ShelfTestCalloc(size_t count, size_t size)
{
    ++allocations;
    if (fail_at != 0U && allocations == fail_at) return NULL;
    return calloc(count, size);
}
#define calloc ShelfTestCalloc
#include "../../src/teacher/learning_pack_copy.c"
#include "../../src/teacher/learning_shelf.c"
#undef calloc

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningShelf *shelf = NULL, *clone = NULL;
    UmiLearningPack *pack = NULL, *candidate = NULL;
    UmiLearningPackInfo info;
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 2U; ++i) {
        CHECK(ShelfFixturePack(root, i == 0U ? "first" : "second",
            "framework", "en", 1U, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
    }
    if (strcmp(name, "clone-allocation") == 0) {
        /* Shelf, first metadata/resources, then second metadata/resources.
         * Fail each allocation in turn, including after one pack was copied. */
        for (size_t failure = 1U; failure <= 5U; ++failure) {
            allocations = 0U; fail_at = failure;
            CHECK(UmiLearningShelfClone(shelf, &clone) == UMI_STATUS_OUT_OF_MEMORY && clone == NULL);
            fail_at = 0U;
            CHECK(shelf->count == 2U && shelf->generation == 2U);
            CHECK(UmiLearningShelfPackAt(shelf, 1U, &info) == UMI_STATUS_OK && strcmp(info.id, "second") == 0);
        }
        CHECK(UmiLearningShelfClone(shelf, &clone) == UMI_STATUS_OK);
        CHECK(clone->count == 2U && clone->generation == 2U);
        goto cleanup;
    }
    CHECK(ShelfFixturePack(root, strcmp(name, "exhausted-adopt") == 0 ? "third" : "first",
        "framework", "en", 2U, &candidate) == UMI_STATUS_OK);
    UmiLearningPack *kept = candidate;
    if (strcmp(name, "replace-no-allocation") == 0) {
        allocations = 0U; fail_at = 1U;
        CHECK(UmiLearningShelfReplace(shelf, 2U, &candidate) == UMI_STATUS_OK);
        CHECK(candidate == NULL && allocations == 0U && shelf->generation == 3U);
        goto cleanup;
    }
    shelf->generation = UINT64_MAX;
    if (strcmp(name, "exhausted-adopt") == 0)
        CHECK(UmiLearningShelfAdopt(shelf, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
    else if (strcmp(name, "exhausted-remove") == 0)
        CHECK(UmiLearningShelfRemove(shelf, "first") == UMI_STATUS_CAPACITY_EXCEEDED);
    else if (strcmp(name, "exhausted-replace") == 0)
        CHECK(UmiLearningShelfReplace(shelf, UINT64_MAX, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
    else if (strcmp(name, "exhausted-clone") == 0) {
        CHECK(UmiLearningShelfClone(shelf, &clone) == UMI_STATUS_OK);
        CHECK(clone->generation == UINT64_MAX && clone->count == 2U);
        CHECK(UmiLearningShelfReplace(clone, UINT64_MAX, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else CHECK(0 && "Unknown shelf fault case");
    CHECK(candidate == kept && shelf->count == 2U && shelf->generation == UINT64_MAX);
    CHECK(UmiLearningShelfPackAt(shelf, 0U, &info) == UMI_STATUS_OK && info.revision == 1U);
cleanup:
    fail_at = 0U;
    UmiLearningPackDestroy(pack); UmiLearningPackDestroy(candidate);
    UmiLearningShelfDestroy(clone); UmiLearningShelfDestroy(shelf);
    return result;
}
