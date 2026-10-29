/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_shelf.c
 *
 * PURPOSE:
 *   Own independently versioned learning packs behind a shared catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_pack_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiLearningShelf {
    UmiLearningPack *packs[UMI_LEARNING_SHELF_MAX_PACKS];
    size_t count;
    uint64_t generation;
};
/* The application serializes shelf changes, then renders copied metadata.
 * Packs own their text and paths, so callers never borrow manifest buffers. */
static size_t ShelfFind(const UmiLearningShelf *shelf, const char *id)
{
    for (size_t i = 0U; i < shelf->count; ++i)
        if (strcmp(shelf->packs[i]->info.id, id) == 0) return i;
    return SIZE_MAX;
}
UmiStatus UmiLearningShelfCreate(UmiLearningShelf **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = calloc(1U, sizeof(**out));
    return *out == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
}
void UmiLearningShelfDestroy(UmiLearningShelf *shelf)
{
    if (shelf == NULL) return;
    for (size_t i = 0U; i < shelf->count; ++i) UmiLearningPackDestroy(shelf->packs[i]);
    free(shelf);
}
/* Preserve the earlier mutation for review. Advance a checked generation so
 * later reviewed replacements can detect an intervening catalogue change. */
#if 0
UmiStatus UmiLearningShelfAdopt(UmiLearningShelf *shelf, UmiLearningPack **pack)
{
    if (shelf == NULL || pack == NULL || *pack == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ShelfFind(shelf, (*pack)->info.id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (shelf->count == UMI_LEARNING_SHELF_MAX_PACKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    shelf->packs[shelf->count++] = *pack;
    *pack = NULL;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiLearningShelfAdopt(UmiLearningShelf *shelf, UmiLearningPack **pack)
{
    if (shelf == NULL || pack == NULL || *pack == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ShelfFind(shelf, (*pack)->info.id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (shelf->count == UMI_LEARNING_SHELF_MAX_PACKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (shelf->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    shelf->packs[shelf->count++] = *pack;
    ++shelf->generation;
    *pack = NULL;
    return UMI_STATUS_OK;
}
/* Preserve the earlier mutation for review. Advance a checked generation so
 * later reviewed replacements can detect an intervening catalogue change. */
#if 0
UmiStatus UmiLearningShelfRemove(UmiLearningShelf *shelf, const char *id)
{
    if (shelf == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiLearningPackDestroy(shelf->packs[index]);
    for (size_t i = index + 1U; i < shelf->count; ++i) shelf->packs[i - 1U] = shelf->packs[i];
    shelf->packs[--shelf->count] = NULL;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiLearningShelfRemove(UmiLearningShelf *shelf, const char *id)
{
    if (shelf == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (shelf->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningPackDestroy(shelf->packs[index]);
    for (size_t i = index + 1U; i < shelf->count; ++i) shelf->packs[i - 1U] = shelf->packs[i];
    shelf->packs[--shelf->count] = NULL;
    ++shelf->generation;
    return UMI_STATUS_OK;
}
size_t UmiLearningShelfCount(const UmiLearningShelf *shelf)
{
    return shelf == NULL ? 0U : shelf->count;
}
UmiStatus UmiLearningShelfPackAt(const UmiLearningShelf *shelf, size_t index,
    UmiLearningPackInfo *out)
{
    if (shelf == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= shelf->count) return UMI_STATUS_NOT_FOUND;
    return UmiLearningPackCopyInfo(shelf->packs[index], out);
}
UmiStatus UmiLearningShelfFindResource(const UmiLearningShelf *shelf, const char *pack_id,
    const char *resource_id, UmiLearningPackResource *out)
{
    if (shelf == NULL || pack_id == NULL || resource_id == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, pack_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiLearningPackFindResource(shelf->packs[index], resource_id, out);
}
UmiStatus UmiLearningShelfReadResource(const UmiLearningShelf *shelf, const char *pack_id,
    const char *resource_id, size_t maximum_bytes, unsigned char **out_bytes, size_t *out_size)
{
    if (out_bytes != NULL) *out_bytes = NULL;
    if (out_size != NULL) *out_size = 0U;
    if (shelf == NULL || pack_id == NULL || resource_id == NULL || out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, pack_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiLearningPackReadResource(shelf->packs[index], resource_id, maximum_bytes, out_bytes, out_size);
}

/* Metadata enumeration lets catalogue views reuse the shelf's ownership rules.
 * Returning a complete value keeps a UI independent of later shelf removal. */
UmiStatus UmiLearningShelfResourceAt(const UmiLearningShelf *shelf, const char *pack_id,
    size_t index, UmiLearningPackResource *out)
{
    if (shelf == NULL || pack_id == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t pack_index = ShelfFind(shelf, pack_id);
    if (pack_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiLearningPackResourceAt(shelf->packs[pack_index], index, out);
}

/* Independent pack ownership lets catalogue and reader lifetimes differ.
 * Keep the deep copy here, beside shelf ownership, so frontends never borrow
 * private arrays or remove a catalogue entry merely to open its first lesson. */
/* Centralise deep-copy ownership for shelf readers and reader transactions.
 * Preserve the earlier shelf-local copy for review. */
#if 0
UmiStatus UmiLearningShelfCopyPack(const UmiLearningShelf *shelf, const char *id,
    UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (shelf == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiLearningPack *source = shelf->packs[index];
    UmiLearningPack *copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy->root, source->root, sizeof(copy->root));
    memcpy(&copy->info, &source->info, sizeof(copy->info));
    if (source->info.resource_count != 0U) {
        copy->resources = calloc(source->info.resource_count, sizeof(*copy->resources));
        if (copy->resources == NULL) {
            free(copy);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        memcpy(copy->resources, source->resources,
            source->info.resource_count * sizeof(*copy->resources));
    }
    *out = copy;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiLearningShelfCopyPack(const UmiLearningShelf *shelf, const char *id,
    UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (shelf == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = ShelfFind(shelf, id);
    return index == SIZE_MAX ? UMI_STATUS_NOT_FOUND : UmiLearningPackClone(shelf->packs[index], out);
}


UmiStatus UmiLearningShelfCopySnapshot(const UmiLearningShelf *shelf, UmiLearningShelfSnapshot *out)
{
    if (shelf == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = (UmiLearningShelfSnapshot){shelf->generation, shelf->count};
    return UMI_STATUS_OK;
}

UmiStatus UmiLearningShelfClone(const UmiLearningShelf *source, UmiLearningShelf **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningShelf *copy = NULL;
    UmiStatus status = UmiLearningShelfCreate(&copy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < source->count; ++i) {
        status = UmiLearningPackClone(source->packs[i], &copy->packs[i]);
        if (status != UMI_STATUS_OK) { UmiLearningShelfDestroy(copy); return status; }
        ++copy->count;
    }
    copy->generation = source->generation;
    *out = copy;
    return UMI_STATUS_OK;
}

static int ShelfLocaleEqual(const char *a, const char *b)
{
    for (size_t i = 0U;; ++i) {
        unsigned char left = (unsigned char)a[i], right = (unsigned char)b[i];
        if (left >= 'A' && left <= 'Z') left = (unsigned char)(left + ('a' - 'A'));
        if (right >= 'A' && right <= 'Z') right = (unsigned char)(right + ('a' - 'A'));
        if (left != right) return 0;
        if (left == 0U) return 1;
    }
}

UmiStatus UmiLearningShelfReplace(UmiLearningShelf *shelf, uint64_t expected_generation,
    UmiLearningPack **candidate)
{
    if (shelf == NULL || candidate == NULL || *candidate == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (shelf->generation != expected_generation) return UMI_STATUS_BUSY;
    size_t index = ShelfFind(shelf, (*candidate)->info.id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiLearningPackInfo *old = &shelf->packs[index]->info, *next = &(*candidate)->info;
    if (strcmp(old->product, next->product) != 0 || !ShelfLocaleEqual(old->locale, next->locale) ||
        next->revision <= old->revision) return UMI_STATUS_INVALID_STATE;
    if (shelf->generation == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* No allocation or filesystem work remains at this publication point.
     * Independently copied readers continue owning their admitted old pack. */
    UmiLearningPack *previous = shelf->packs[index];
    shelf->packs[index] = *candidate;
    *candidate = NULL;
    ++shelf->generation;
    UmiLearningPackDestroy(previous);
    return UMI_STATUS_OK;
}
