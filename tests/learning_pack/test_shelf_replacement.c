/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_shelf_replacement.c
 *
 * PURPOSE:
 *   Verify reviewed replacement preserves ownership, order and older reader sessions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "shelf_fixture.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/teacher/learning_search.h"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningShelf *shelf = NULL, *clone = NULL;
    UmiLearningPack *pack = NULL, *candidate = NULL, *copy = NULL;
    UmiLearningReader *reader = NULL, *new_reader = NULL;
    UmiLearningSearch *search = NULL;
    UmiLearningShelfSnapshot before, after;
    UmiLearningPackInfo info;
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    CHECK(UmiLearningShelfCopySnapshot(shelf, &before) == UMI_STATUS_OK);
    CHECK(before.generation == 0U && before.pack_count == 0U);
    if (strcmp(name, "empty-clone") == 0) {
        CHECK(UmiLearningShelfClone(shelf, &clone) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfCopySnapshot(clone, &after) == UMI_STATUS_OK);
        CHECK(after.generation == 0U && after.pack_count == 0U);
        goto cleanup;
    }
    CHECK(ShelfFixturePack(root, "guide", "framework", "en-GB", 2U, &pack) == UMI_STATUS_OK);
    CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK && pack == NULL);
    CHECK(UmiLearningShelfCopySnapshot(shelf, &before) == UMI_STATUS_OK);
    CHECK(before.generation == 1U && before.pack_count == 1U);
    const char *id = strcmp(name, "missing-id") == 0 ? "absent" : "guide";
    const char *product = strcmp(name, "product-change") == 0 ? "studio" : "framework";
    const char *locale = strcmp(name, "locale-change") == 0 ? "fr" :
        strcmp(name, "locale-case") == 0 ? "EN-gb" : "en-GB";
    uint64_t revision = strcmp(name, "same-revision") == 0 ? 2U :
        strcmp(name, "older-revision") == 0 ? 1U : 3U;
    CHECK(ShelfFixturePack(root, id, product, locale, revision, &candidate) == UMI_STATUS_OK);
    UmiLearningPack *kept = candidate;
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningShelfReplace(NULL, 1U, &candidate) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningShelfReplace(shelf, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningShelfReplace(shelf, 1U, &pack) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningShelfCopySnapshot(NULL, &after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningShelfCopySnapshot(shelf, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningShelfClone(NULL, &clone) == UMI_STATUS_INVALID_ARGUMENT && clone == NULL);
        CHECK(UmiLearningShelfClone(shelf, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(candidate == kept);
        goto unchanged;
    }
    if (strcmp(name, "duplicate-adopt") == 0) {
        CHECK(UmiLearningShelfAdopt(shelf, &candidate) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(candidate == kept);
        goto unchanged;
    }
    if (strcmp(name, "missing-remove") == 0) {
        CHECK(UmiLearningShelfRemove(shelf, "absent") == UMI_STATUS_NOT_FOUND);
        goto unchanged;
    }
    if (strcmp(name, "stale") == 0 || strcmp(name, "future") == 0) {
        CHECK(UmiLearningShelfReplace(shelf, strcmp(name, "stale") == 0 ? 0U : 2U,
            &candidate) == UMI_STATUS_BUSY);
        CHECK(candidate == kept);
        goto unchanged;
    }
    if (strcmp(name, "same-revision") == 0 || strcmp(name, "older-revision") == 0 ||
        strcmp(name, "product-change") == 0 || strcmp(name, "locale-change") == 0 ||
        strcmp(name, "missing-id") == 0) {
        UmiStatus expected = strcmp(name, "missing-id") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE;
        CHECK(UmiLearningShelfReplace(shelf, before.generation, &candidate) == expected);
        CHECK(candidate == kept);
        goto unchanged;
    }
    if (strcmp(name, "clone-lifetime") == 0 || strcmp(name, "clone-independent") == 0) {
        CHECK(UmiLearningShelfClone(shelf, &clone) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfCopySnapshot(clone, &after) == UMI_STATUS_OK);
        CHECK(after.generation == before.generation && after.pack_count == before.pack_count);
        if (strcmp(name, "clone-lifetime") == 0) {
            UmiLearningShelfDestroy(shelf); shelf = NULL;
        } else {
            CHECK(UmiLearningShelfRemove(shelf, "guide") == UMI_STATUS_OK);
            CHECK(UmiLearningShelfCopySnapshot(shelf, &after) == UMI_STATUS_OK);
            CHECK(after.generation == 2U && after.pack_count == 0U);
        }
        CHECK(UmiLearningShelfPackAt(clone, 0U, &info) == UMI_STATUS_OK && info.revision == 2U);
        CHECK(UmiLearningShelfReplace(clone, before.generation, &candidate) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfPackAt(clone, 0U, &info) == UMI_STATUS_OK && info.revision == 3U);
        goto cleanup;
    }
    if (strcmp(name, "full-shelf") == 0 || strcmp(name, "middle-order") == 0) {
        size_t count = strcmp(name, "full-shelf") == 0 ? UMI_LEARNING_SHELF_MAX_PACKS : 3U;
        for (size_t i = 1U; i < count; ++i) {
            char extra[32];
            (void)snprintf(extra, sizeof(extra), "other-%zu", i);
            CHECK(ShelfFixturePack(root, extra, "framework", "en", 1U, &pack) == UMI_STATUS_OK);
            CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK);
        }
        if (strcmp(name, "middle-order") == 0) {
            UmiLearningPackDestroy(candidate); candidate = NULL;
            CHECK(ShelfFixturePack(root, "other-1", "framework", "en", 2U, &candidate) == UMI_STATUS_OK);
        }
        CHECK(UmiLearningShelfCopySnapshot(shelf, &before) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfReplace(shelf, before.generation, &candidate) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfCount(shelf) == count);
        for (size_t i = 0U; i < count; ++i) {
            char expected[32] = "guide";
            if (i != 0U) (void)snprintf(expected, sizeof(expected), "other-%zu", i);
            CHECK(UmiLearningShelfPackAt(shelf, i, &info) == UMI_STATUS_OK);
            CHECK(strcmp(info.id, expected) == 0);
        }
        goto advanced;
    }
    if (strcmp(name, "reader-kept") == 0 || strcmp(name, "search-stale") == 0) {
        CHECK(UmiRootedFileWrite(root, "lesson.txt", "lesson", 6U) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfCopyPack(shelf, "guide", &copy) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderCreate(&copy, 64U, &reader) == UMI_STATUS_OK);
        CHECK(UmiLearningReaderOpen(reader, "lesson") == UMI_STATUS_OK);
        UmiLearningSearchQuery query = {0};
        CHECK(UmiLearningSearchCreate(shelf, &query, 4U, &search) == UMI_STATUS_OK);
    } else CHECK(strcmp(name, "replace") == 0 || strcmp(name, "locale-case") == 0);
    CHECK(UmiLearningShelfReplace(shelf, before.generation, &candidate) == UMI_STATUS_OK);
    CHECK(candidate == NULL);
    CHECK(UmiLearningShelfPackAt(shelf, 0U, &info) == UMI_STATUS_OK && info.revision == 3U);
    if (reader != NULL) {
        UmiLearningReaderSnapshot state;
        CHECK(UmiLearningReaderCopySnapshot(reader, &state) == UMI_STATUS_OK);
        CHECK(state.pack.revision == 2U && state.has_document && state.content_size == 6U);
        if (strcmp(name, "reader-kept") == 0) {
            UmiLearningShelfDestroy(shelf); shelf = NULL;
            CHECK(UmiLearningReaderReload(reader) == UMI_STATUS_OK);
            goto cleanup;
        }
        UmiLearningSearchHit hit;
        CHECK(UmiLearningSearchResultAt(search, 0U, &hit) == UMI_STATUS_OK && hit.pack.revision == 2U);
        CHECK(UmiLearningReaderCreateFromShelf(shelf, hit.pack.id, hit.pack.revision,
            hit.resource.id, hit.resource.sha256, 64U, &new_reader) == UMI_STATUS_INVALID_STATE);
        CHECK(new_reader == NULL);
    }
advanced:
    CHECK(UmiLearningShelfCopySnapshot(shelf, &after) == UMI_STATUS_OK);
    CHECK(after.generation == before.generation + 1U && after.pack_count == before.pack_count);
    goto cleanup;
unchanged:
    CHECK(UmiLearningShelfCopySnapshot(shelf, &after) == UMI_STATUS_OK);
    CHECK(after.generation == before.generation && after.pack_count == before.pack_count);
    CHECK(UmiLearningShelfPackAt(shelf, 0U, &info) == UMI_STATUS_OK && info.revision == 2U);
cleanup:
    UmiLearningSearchDestroy(search);
    UmiLearningReaderDestroy(reader); UmiLearningReaderDestroy(new_reader);
    UmiLearningPackDestroy(pack); UmiLearningPackDestroy(candidate); UmiLearningPackDestroy(copy);
    UmiLearningShelfDestroy(shelf); UmiLearningShelfDestroy(clone);
    return result;
}
