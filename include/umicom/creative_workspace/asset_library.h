/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/asset_library.h
 * PURPOSE: Own an ordered collection of complete creative assets with portable identities and bounded memory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_ASSET_LIBRARY_H
#define UMICOM_CREATIVE_WORKSPACE_ASSET_LIBRARY_H
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CREATIVE_LIBRARY_MAX_ASSETS 32U
#define UMI_CREATIVE_LIBRARY_MAX_BYTES UMI_CREATIVE_ASSET_MAX_BYTES
    typedef struct UmiCreativeAssetLibrary UmiCreativeAssetLibrary;
    typedef struct UmiCreativeAssetLibraryInfo
    {
        char title[UMI_CREATIVE_LABEL_CAPACITY];
        size_t asset_count, byte_count;
    } UmiCreativeAssetLibraryInfo;
    typedef struct UmiCreativeAssetLibraryEntry
    {
        char id[UMI_CREATIVE_ID_CAPACITY];
        UmiCreativeAssetInfo asset;
    } UmiCreativeAssetLibraryEntry;
    /* A library has one owner thread. Creation performs no I/O. Initialise *out=NULL;
 * errors preserve outputs, including an already-live owner. Title is nonempty,
 * single-line UTF-8. The aggregate payload limit is independent of asset count. */
    UmiStatus UmiCreativeAssetLibraryCreate(const char *title, UmiCreativeAssetLibrary **out);
    void UmiCreativeAssetLibraryDestroy(UmiCreativeAssetLibrary *library);
    UmiStatus UmiCreativeAssetLibraryInspect(const UmiCreativeAssetLibrary *library,
                                             UmiCreativeAssetLibraryInfo *out);
    UmiStatus UmiCreativeAssetLibraryTitle(UmiCreativeAssetLibrary *library, const char *title);
    /* Insert takes ownership only on success and sets *asset=NULL. IDs use the same
 * bounded ASCII rules as scene element IDs. They are case-sensitive identities,
 * never file paths. Existing IDs are refused; nothing is silently replaced.
 * Two different IDs may intentionally refer to equal content. */
    UmiStatus UmiCreativeAssetLibraryInsert(UmiCreativeAssetLibrary *library, const char *id,
                                            UmiCreativeAsset **asset);
    UmiStatus UmiCreativeAssetLibraryAt(const UmiCreativeAssetLibrary *library, size_t index,
                                        UmiCreativeAssetLibraryEntry *out);
    /* Borrow is valid until Detach/Destroy. Mutation and worker access must not race.
 * Callers validate the media format before decoding or sending these bytes. */
    UmiStatus UmiCreativeAssetLibraryBorrow(const UmiCreativeAssetLibrary *library, const char *id,
                                            const UmiCreativeAsset **out);
    /* Detach transfers the selected capture to a NULL output, leaving disk files
 * untouched. Hosts may keep this owner for Undo or explicitly destroy it. */
    UmiStatus UmiCreativeAssetLibraryDetach(UmiCreativeAssetLibrary *library, const char *id,
                                            UmiCreativeAsset **out);
    UmiStatus UmiCreativeAssetLibraryMove(UmiCreativeAssetLibrary *library, const char *id, size_t index);
#ifdef __cplusplus
}
#endif
#endif
