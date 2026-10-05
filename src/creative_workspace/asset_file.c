/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_file.c
 * PURPOSE: Compose asset capture and explicit new-file copies from the existing native input/output owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_internal.h"
#include "umicom/platform/input_file.h"
#include <string.h>

UmiStatus UmiCreativeAssetLoadFile(const char *path, const char *label, UmiCreativeAssetKind kind,
                                   size_t maximum_bytes, const UmiCancellationToken *cancel,
                                   UmiCreativeAsset **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (maximum_bytes > UMI_CREATIVE_ASSET_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    UmiCreativeAsset *asset = NULL;
    status = UmiCreativeAssetPrepare(label, kind, cancel, &asset);
    if (status != UMI_STATUS_OK)
        return status;
    /* Transfer the reader's owned buffer directly, avoiding a second full copy
     * while keeping file I/O and mutation checks in the Platform module. */
    asset->release_bytes = UmiInputFileFree;
    status = UmiInputFileRead(path, maximum_bytes, &asset->bytes, &asset->info.byte_count);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        memcpy(asset->info.source_path, path, strlen(path) + 1U);
        *out = asset;
    }
    else
        UmiCreativeAssetDestroy(asset);
    return status;
}
UmiStatus UmiCreativeAssetWriteNew(const UmiCreativeAsset *asset, const char *path,
                                   const UmiCancellationToken *cancel, UmiCreativeAssetWriteResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (asset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiOutputFile *file = NULL;
    status = UmiOutputFileCreate(path, &file);
    if (status != UMI_STATUS_OK)
        return status;
    out->created = true;
    for (size_t offset = 0U; status == UMI_STATUS_OK && offset < asset->info.byte_count;)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        size_t count = asset->info.byte_count - offset;
        if (count > 65536U)
            count = 65536U;
        status = UmiOutputFileWrite(file, asset->bytes + offset, count);
        offset += count;
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
        status = closed;
    UmiStatus inspected = UmiOutputFileRead(file, &out->file);
    if (status == UMI_STATUS_OK)
        status = inspected;
    UmiOutputFileDestroy(file);
    return status;
}
