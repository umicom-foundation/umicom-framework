/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_library.c
 * PURPOSE: Centralise creative asset membership, ordering and ownership without duplicating captured bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_library_internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

static size_t Find(const UmiCreativeAssetLibrary *library, const char *id)
{
    for (size_t i = 0U; i < library->info.asset_count; ++i)
        if (strcmp(library->slots[i].id, id) == 0)
            return i;
    return library->info.asset_count;
}
UmiStatus UmiCreativeAssetLibraryCreate(const char *title, UmiCreativeAssetLibrary **out)
{
    if (out == NULL || !UmiCreativeTextValid(title, UMI_CREATIVE_LABEL_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiCreativeAssetLibrary *library = calloc(1U, sizeof(*library));
    if (library == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(library->info.title, title, strlen(title) + 1U);
    *out = library;
    return UMI_STATUS_OK;
}
void UmiCreativeAssetLibraryDestroy(UmiCreativeAssetLibrary *library)
{
    if (library == NULL)
        return;
    for (size_t i = 0U; i < library->info.asset_count; ++i)
        UmiCreativeAssetDestroy(library->slots[i].asset);
    free(library);
}
UmiStatus UmiCreativeAssetLibraryInspect(const UmiCreativeAssetLibrary *library,
                                         UmiCreativeAssetLibraryInfo *out)
{
    if (library == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = library->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryTitle(UmiCreativeAssetLibrary *library, const char *title)
{
    if (library == NULL || !UmiCreativeTextValid(title, UMI_CREATIVE_LABEL_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The title is metadata, not a filename. Overlap is permitted when a host
     * uses a previously borrowed buffer to supply the same title again. */
    memmove(library->info.title, title, strlen(title) + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryInsert(UmiCreativeAssetLibrary *library, const char *id,
                                        UmiCreativeAsset **asset)
{
    if (library == NULL || asset == NULL || *asset == NULL || !UmiCreativeIdValid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Find(library, id) != library->info.asset_count)
        return UMI_STATUS_ALREADY_EXISTS;
    if (library->info.asset_count == UMI_CREATIVE_LIBRARY_MAX_ASSETS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeAssetInfo info;
    UmiStatus status = UmiCreativeAssetInspect(*asset, &info);
    if (status != UMI_STATUS_OK)
        return status;
    if (info.byte_count > UMI_CREATIVE_LIBRARY_MAX_BYTES - library->info.byte_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A transferred pointer has exactly one owner. Reject accidental insertion
     * of an already-owned capture even when the caller supplies a different ID. */
    for (size_t i = 0U; i < library->info.asset_count; ++i)
        if (library->slots[i].asset == *asset)
            return UMI_STATUS_INVALID_STATE;
    UmiCreativeLibrarySlot *slot = &library->slots[library->info.asset_count];
    memcpy(slot->id, id, strlen(id) + 1U);
    slot->asset = *asset;
    *asset = NULL;
    ++library->info.asset_count;
    library->info.byte_count += info.byte_count;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryAt(const UmiCreativeAssetLibrary *library, size_t index,
                                    UmiCreativeAssetLibraryEntry *out)
{
    if (library == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= library->info.asset_count)
        return UMI_STATUS_NOT_FOUND;
    UmiCreativeAssetLibraryEntry entry = {0};
    memcpy(entry.id, library->slots[index].id, strlen(library->slots[index].id) + 1U);
    UmiStatus status = UmiCreativeAssetInspect(library->slots[index].asset, &entry.asset);
    if (status == UMI_STATUS_OK)
        *out = entry;
    return status;
}
UmiStatus UmiCreativeAssetLibraryBorrow(const UmiCreativeAssetLibrary *library, const char *id,
                                        const UmiCreativeAsset **out)
{
    if (library == NULL || out == NULL || !UmiCreativeIdValid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = Find(library, id);
    if (index == library->info.asset_count)
        return UMI_STATUS_NOT_FOUND;
    *out = library->slots[index].asset;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryDetach(UmiCreativeAssetLibrary *library, const char *id,
                                        UmiCreativeAsset **out)
{
    if (library == NULL || out == NULL || !UmiCreativeIdValid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    size_t index = Find(library, id);
    if (index == library->info.asset_count)
        return UMI_STATUS_NOT_FOUND;
    UmiCreativeAssetInfo info;
    UmiStatus status = UmiCreativeAssetInspect(library->slots[index].asset, &info);
    if (status != UMI_STATUS_OK)
        return status;
    *out = library->slots[index].asset;
    --library->info.asset_count;
    library->info.byte_count -= info.byte_count;
    memmove(&library->slots[index], &library->slots[index + 1U],
            (library->info.asset_count - index) * sizeof(library->slots[0]));
    memset(&library->slots[library->info.asset_count], 0, sizeof(library->slots[0]));
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryMove(UmiCreativeAssetLibrary *library, const char *id, size_t index)
{
    if (library == NULL || !UmiCreativeIdValid(id))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t previous = Find(library, id);
    if (previous == library->info.asset_count || index >= library->info.asset_count)
        return UMI_STATUS_NOT_FOUND;
    if (previous == index)
        return UMI_STATUS_OK;
    UmiCreativeLibrarySlot moving = library->slots[previous];
    /* Move complete slots, including their owners. There is no byte copy and
     * callers continue to identify assets by ID rather than a changing row. */
    if (previous < index)
        memmove(&library->slots[previous], &library->slots[previous + 1U],
                (index - previous) * sizeof(moving));
    else
        memmove(&library->slots[index + 1U], &library->slots[index], (previous - index) * sizeof(moving));
    library->slots[index] = moving;
    return UMI_STATUS_OK;
}
