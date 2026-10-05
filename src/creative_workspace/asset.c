/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset.c
 * PURPOSE: Own complete immutable attachment bytes and meaningful labels for reuse across creative applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiCreativeAssetPrepare(const char *label, UmiCreativeAssetKind kind,
                                  const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!UmiCreativeTextValid(label, UMI_CREATIVE_LABEL_CAPACITY, false) ||
        kind < UMI_CREATIVE_ASSET_DOCUMENT || kind > UMI_CREATIVE_ASSET_BINARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiCreativeAsset *asset = calloc(1U, sizeof(*asset));
    if (asset == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(asset->info.label, label, strlen(label) + 1U);
    asset->info.declared_kind = kind;
    asset->release_bytes = free;
    *out = asset;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetCapture(const char *label, UmiCreativeAssetKind kind, const void *bytes,
                                  size_t size, const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (out == NULL || (bytes == NULL && size != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (size > UMI_CREATIVE_ASSET_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiCreativeAsset *asset = NULL;
    UmiStatus status = UmiCreativeAssetPrepare(label, kind, cancel, &asset);
    if (status != UMI_STATUS_OK)
        return status;
    asset->bytes = malloc(size + 1U);
    if (asset->bytes == NULL)
    {
        UmiCreativeAssetDestroy(asset);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Copy in bounded chunks so cancellation can retire a large capture before
     * it is published. Only the final assignment transfers ownership. */
    for (size_t offset = 0U; offset < size;)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            UmiCreativeAssetDestroy(asset);
            return UMI_STATUS_CANCELLED;
        }
        size_t count = size - offset;
        if (count > 65536U)
            count = 65536U;
        memcpy(asset->bytes + offset, (const unsigned char *)bytes + offset, count);
        offset += count;
    }
    if (umi_cancellation_token_is_requested(cancel))
    {
        UmiCreativeAssetDestroy(asset);
        return UMI_STATUS_CANCELLED;
    }
    asset->bytes[size] = 0U;
    asset->info.byte_count = size;
    *out = asset;
    return UMI_STATUS_OK;
}
void UmiCreativeAssetDestroy(UmiCreativeAsset *asset)
{
    if (asset != NULL)
    {
        asset->release_bytes(asset->bytes);
        free(asset);
    }
}
UmiStatus UmiCreativeAssetInspect(const UmiCreativeAsset *asset, UmiCreativeAssetInfo *out)
{
    if (asset == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = asset->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetBytes(const UmiCreativeAsset *asset, const void **out_bytes, size_t *out_size)
{
    if (out_bytes != NULL)
        *out_bytes = NULL;
    if (out_size != NULL)
        *out_size = 0U;
    if (asset == NULL || out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_bytes = asset->bytes;
    *out_size = asset->info.byte_count;
    return UMI_STATUS_OK;
}
