/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_archive_file.c
 * PURPOSE: Store and reopen complete asset envelopes through the shared native file owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_archive_internal.h"
#include "umicom/platform/input_file.h"
#include <string.h>

static UmiStatus WriteChunks(UmiOutputFile *file, const void *bytes, size_t size,
                             const UmiCancellationToken *cancel)
{
    for (size_t offset = 0U; offset < size;)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        size_t count = size - offset;
        if (count > 65536U)
            count = 65536U;
        UmiStatus status = UmiOutputFileWrite(file, (const unsigned char *)bytes + offset, count);
        if (status != UMI_STATUS_OK)
            return status;
        offset += count;
    }
    return umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetArchiveSaveNew(const UmiCreativeAsset *asset, const char *path,
                                         const UmiCancellationToken *cancel, UmiCreativeAssetWriteResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    UmiCreativeAssetEnvelope envelope;
    status = UmiCreativeAssetArchiveDescribe(asset, cancel, &envelope);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiOutputFile *file = NULL;
    status = UmiOutputFileCreate(path, &file);
    if (status != UMI_STATUS_OK)
        return status;
    out->created = true;
    /* Stream the already-owned payload without allocating a second 64 MiB
     * buffer. The final digest makes incomplete archives fail closed on load. */
    status = WriteChunks(file, envelope.metadata, envelope.metadata_size, cancel);
    if (status == UMI_STATUS_OK)
        status = WriteChunks(file, envelope.payload, envelope.payload_size, cancel);
    if (status == UMI_STATUS_OK)
        status = WriteChunks(file, envelope.digest, sizeof(envelope.digest), cancel);
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
        status = closed;
    UmiStatus inspected = UmiOutputFileRead(file, &out->file);
    if (status == UMI_STATUS_OK)
        status = inspected;
    UmiOutputFileDestroy(file);
    return status;
}
UmiStatus UmiCreativeAssetArchiveLoad(const char *path, size_t maximum_bytes,
                                      const UmiCancellationToken *cancel, UmiCreativeAsset **out)
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
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    size_t limit = maximum_bytes + UMI_CREATIVE_LABEL_CAPACITY - 1U + 56U;
    status = UmiInputFileRead(path, limit, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetArchiveDecodeBounded(bytes, size, maximum_bytes, cancel, out);
    UmiInputFileFree(bytes);
    return status;
}
