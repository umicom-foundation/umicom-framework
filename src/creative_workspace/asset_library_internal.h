/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_library_internal.h
 * PURPOSE: Keep asset ownership and storage framing private to the creative library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_ASSET_LIBRARY_INTERNAL_H
#define UMICOM_CREATIVE_ASSET_LIBRARY_INTERNAL_H
#include "umicom/creative_workspace/asset_library.h"
typedef struct UmiCreativeLibrarySlot
{
    char id[UMI_CREATIVE_ID_CAPACITY];
    UmiCreativeAsset *asset;
} UmiCreativeLibrarySlot;
struct UmiCreativeAssetLibrary
{
    UmiCreativeAssetLibraryInfo info;
    UmiCreativeLibrarySlot slots[UMI_CREATIVE_LIBRARY_MAX_ASSETS];
};
#endif
