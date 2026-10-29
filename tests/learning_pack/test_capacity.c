/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_capacity.c
 *
 * PURPOSE:
 *   Exercise exact manifest, resource, shelf and search result capacity boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)

static UmiStatus BuildManifest(const char *id, unsigned count, char **out, size_t *out_size)
{
    *out = NULL; *out_size = 0U;
    size_t capacity = UMI_LEARNING_PACK_MANIFEST_BYTES + 2U;
    char *json = calloc(capacity, 1U);
    if (json == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    int length = snprintf(json, capacity,
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Capacity guide\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":[", id);
    if (length < 0 || (size_t)length >= capacity) { free(json); return UMI_STATUS_CAPACITY_EXCEEDED; }
    size_t used = (size_t)length;
    /* Fixture metadata is generated from integers and fixed literals, so it
     * cannot introduce unescaped JSON text. Resource IDs remain distinct. */
    for (unsigned i = 0U; i < count; ++i) {
        length = snprintf(json + used, capacity - used,
            "%s{\"id\":\"resource.%u\",\"title\":\"Resource %u\",\"topic\":\"capacity\","
            "\"kind\":\"example\",\"path\":\"source-%u.c\","
            "\"sha256\":\"0000000000000000000000000000000000000000000000000000000000000000\"}",
            i == 0U ? "" : ",", i, i, i);
        if (length < 0 || (size_t)length >= capacity - used) {
            free(json); return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)length;
    }
    if (capacity - used < 3U) { free(json); return UMI_STATUS_CAPACITY_EXCEEDED; }
    memcpy(json + used, "]}", 3U);
    *out = json; *out_size = used + 2U;
    return UMI_STATUS_OK;
}
static UmiStatus CreatePack(const char *root, const char *id, unsigned count, UmiLearningPack **out)
{
    char *json = NULL;
    size_t size = 0U;
    UmiStatus status = BuildManifest(id, count, &json, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackCreate(json, size, root, (UmiVersion){0U,9U,0U}, out);
    free(json);
    return status;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    int result = 0;
    const char *mode = argv[1], *root = argv[2];
    UmiLearningPack *pack = NULL;
    UmiLearningShelf *shelf = NULL;
    UmiLearningSearch *search = NULL;
    char *json = NULL;
    size_t size = 0U;
    if (strcmp(mode, "resources-below") == 0 || strcmp(mode, "resources-exact") == 0 ||
        strcmp(mode, "resources-over") == 0) {
        unsigned count = strcmp(mode, "resources-below") == 0 ? UMI_LEARNING_PACK_MAX_RESOURCES - 1U :
            strcmp(mode, "resources-exact") == 0 ? UMI_LEARNING_PACK_MAX_RESOURCES : UMI_LEARNING_PACK_MAX_RESOURCES + 1U;
        UmiStatus status = CreatePack(root, "capacity.pack", count, &pack);
        if (count > UMI_LEARNING_PACK_MAX_RESOURCES) {
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && pack == NULL);
        } else {
            CHECK(status == UMI_STATUS_OK);
            UmiLearningPackInfo info;
            UmiLearningPackResource resource;
            CHECK(UmiLearningPackCopyInfo(pack, &info) == UMI_STATUS_OK && info.resource_count == count);
            CHECK(UmiLearningPackResourceAt(pack, (size_t)count - 1U, &resource) == UMI_STATUS_OK);
            char expected[64];
            int n = snprintf(expected, sizeof(expected), "resource.%u", count - 1U);
            CHECK(n > 0 && (size_t)n < sizeof(expected) && strcmp(resource.id, expected) == 0);
            CHECK(UmiLearningPackResourceAt(pack, count, &resource) == UMI_STATUS_NOT_FOUND);
        }
    } else if (strcmp(mode, "manifest-exact") == 0 || strcmp(mode, "manifest-over") == 0) {
        CHECK(BuildManifest("capacity.pack", 1U, &json, &size) == UMI_STATUS_OK);
        size_t chosen = UMI_LEARNING_PACK_MANIFEST_BYTES + (strcmp(mode, "manifest-over") == 0 ? 1U : 0U);
        CHECK(size < chosen);
        memset(json + size, ' ', chosen - size);
        json[chosen] = '\0';
        UmiStatus status = UmiLearningPackCreate(json, chosen, root, (UmiVersion){0U,9U,0U}, &pack);
        CHECK(status == (chosen == UMI_LEARNING_PACK_MANIFEST_BYTES ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK((pack != NULL) == (status == UMI_STATUS_OK));
    } else if (strcmp(mode, "shelf") == 0 || strcmp(mode, "search-exact") == 0 ||
               strcmp(mode, "search-truncated") == 0 || strcmp(mode, "max-shelf-search") == 0) {
        CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
        unsigned packs = strcmp(mode, "shelf") == 0 || strcmp(mode, "max-shelf-search") == 0 ?
            UMI_LEARNING_SHELF_MAX_PACKS : strcmp(mode, "search-exact") == 0 ? 1U : 2U;
        unsigned resources = strcmp(mode, "shelf") == 0 ? 1U : UMI_LEARNING_PACK_MAX_RESOURCES;
        for (unsigned i = 0U; i < packs; ++i) {
            char id[64];
            int n = snprintf(id, sizeof(id), "pack.%u", i);
            CHECK(n > 0 && (size_t)n < sizeof(id));
            CHECK(CreatePack(root, id, resources, &pack) == UMI_STATUS_OK);
            CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK && pack == NULL);
        }
        if (strcmp(mode, "shelf") == 0) {
            CHECK(CreatePack(root, "pack.extra", 1U, &pack) == UMI_STATUS_OK);
            CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_CAPACITY_EXCEEDED && pack != NULL);
            CHECK(UmiLearningShelfCount(shelf) == UMI_LEARNING_SHELF_MAX_PACKS);
            CHECK(UmiLearningShelfRemove(shelf, "pack.7") == UMI_STATUS_OK);
            CHECK(UmiLearningShelfAdopt(shelf, &pack) == UMI_STATUS_OK && pack == NULL);
            UmiLearningPackInfo info;
            CHECK(UmiLearningShelfPackAt(shelf, UMI_LEARNING_SHELF_MAX_PACKS - 1U, &info) == UMI_STATUS_OK);
            CHECK(strcmp(info.id, "pack.extra") == 0);
        } else {
            UmiLearningSearchQuery query = {0};
            CHECK(UmiLearningSearchCreate(shelf, &query, UMI_LEARNING_SEARCH_MAX_RESULTS, &search) == UMI_STATUS_OK);
            UmiLearningSearchInfo info;
            CHECK(UmiLearningSearchCopyInfo(search, &info) == UMI_STATUS_OK);
            size_t total = (size_t)packs * resources;
            CHECK(info.examined_resources == total && info.matching_resources == total);
            CHECK(info.result_count == UMI_LEARNING_SEARCH_MAX_RESULTS && info.truncated == (packs != 1U));
            UmiLearningSearchHit hit;
            CHECK(UmiLearningSearchResultAt(search, UMI_LEARNING_SEARCH_MAX_RESULTS - 1U, &hit) == UMI_STATUS_OK);
            CHECK(strcmp(hit.pack.id, "pack.0") == 0 && strcmp(hit.resource.id, "resource.255") == 0);
        }
    } else result = 2;
cleanup:
    free(json);
    UmiLearningPackDestroy(pack);
    UmiLearningSearchDestroy(search);
    UmiLearningShelfDestroy(shelf);
    return result;
}
