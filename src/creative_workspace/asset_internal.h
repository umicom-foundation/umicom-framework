/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_internal.h
 * PURPOSE: Keep captured asset storage owned by Framework while public callers receive immutable views.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_ASSET_INTERNAL_H
#define UMICOM_CREATIVE_ASSET_INTERNAL_H
#include "umicom/creative_workspace/asset.h"
struct UmiCreativeAsset
{
    UmiCreativeAssetInfo info;
    unsigned char *bytes;
    void (*release_bytes)(void *);
};
UmiStatus UmiCreativeAssetPrepare(const char *label, UmiCreativeAssetKind kind,
                                  const UmiCancellationToken *cancel, UmiCreativeAsset **out);
#endif
