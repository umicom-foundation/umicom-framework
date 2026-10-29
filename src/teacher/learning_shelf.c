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
UmiStatus UmiLearningShelfAdopt(UmiLearningShelf *shelf, UmiLearningPack **pack)
{
    if (shelf == NULL || pack == NULL || *pack == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (ShelfFind(shelf, (*pack)->info.id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS;
    if (shelf->count == UMI_LEARNING_SHELF_MAX_PACKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    shelf->packs[shelf->count++] = *pack;
    *pack = NULL;
    return UMI_STATUS_OK;
}
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
