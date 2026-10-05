/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_archive.c
 * PURPOSE: Encode portable asset envelopes with explicit byte order, bounded parsing and the existing Framework integrity digest.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_archive_internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

/* These bytes describe a storage format, not an application release. Explicit
 * fields avoid persisting compiler padding, pointer values or native enums. */
static const unsigned char archive_magic[8] = {'U', 'M', 'I', 'A', 'S', 'S', 'E', 'T'};
static void PutSize(unsigned char *out, uint64_t value, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
    {
        out[i] = (unsigned char)(value & 255U);
        value >>= 8U;
    }
}
static uint64_t GetSize(const unsigned char *bytes, size_t count)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < count; ++i)
        value |= (uint64_t)bytes[i] << (8U * i);
    return value;
}
static UmiStatus HashBytes(UmiNativeSha256 *hash, const void *bytes, size_t size,
                           const UmiCancellationToken *cancel)
{
    /* Hashing belongs to the shared digest owner. Chunks let large captures
     * respond to cancellation without adding a second digest implementation. */
    for (size_t offset = 0U; offset < size;)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        size_t count = size - offset;
        if (count > 65536U)
            count = 65536U;
        UmiStatus status = UmiNativeSha256Update(hash, (const unsigned char *)bytes + offset, count);
        if (status != UMI_STATUS_OK)
            return status;
        offset += count;
    }
    return umi_cancellation_token_is_requested(cancel) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetArchiveDescribe(const UmiCreativeAsset *asset, const UmiCancellationToken *cancel,
                                          UmiCreativeAssetEnvelope *out)
{
    UmiCreativeAssetInfo info;
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiCreativeAssetInspect(asset, &info);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    memset(out, 0, sizeof(*out));
    status = UmiCreativeAssetBytes(asset, &out->payload, &out->payload_size);
    if (status != UMI_STATUS_OK)
        return status;
    size_t label_size = strlen(info.label);
    memcpy(out->metadata, archive_magic, sizeof(archive_magic));
    PutSize(out->metadata + 8U, 1U, 2U); /* First supported envelope schema. */
    out->metadata[10U] = (unsigned char)info.declared_kind;
    PutSize(out->metadata + 12U, (uint64_t)label_size, 2U);
    PutSize(out->metadata + 16U, (uint64_t)out->payload_size, 8U);
    memcpy(out->metadata + UMI_ASSET_ARCHIVE_HEADER_BYTES, info.label, label_size);
    out->metadata_size = UMI_ASSET_ARCHIVE_HEADER_BYTES + label_size;
    UmiNativeSha256 hash;
    UmiNativeSha256Init(&hash);
    status = HashBytes(&hash, out->metadata, out->metadata_size, cancel);
    if (status == UMI_STATUS_OK)
        status = HashBytes(&hash, out->payload, out->payload_size, cancel);
    if (status == UMI_STATUS_OK)
        status = UmiNativeSha256Final(&hash, out->digest);
    return status;
}
UmiStatus UmiCreativeAssetArchiveEncode(const UmiCreativeAsset *asset, const UmiCancellationToken *cancel,
                                        unsigned char **out_bytes, size_t *out_size)
{
    if (out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_bytes != NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiCreativeAssetEnvelope envelope;
    UmiStatus status = UmiCreativeAssetArchiveDescribe(asset, cancel, &envelope);
    if (status != UMI_STATUS_OK)
        return status;
    size_t size = envelope.metadata_size + envelope.payload_size + sizeof(envelope.digest);
    unsigned char *bytes = malloc(size);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(bytes, envelope.metadata, envelope.metadata_size);
    for (size_t offset = 0U; offset < envelope.payload_size;)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            free(bytes);
            return UMI_STATUS_CANCELLED;
        }
        size_t count = envelope.payload_size - offset;
        if (count > 65536U)
            count = 65536U;
        memcpy(bytes + envelope.metadata_size + offset, (const unsigned char *)envelope.payload + offset,
               count);
        offset += count;
    }
    memcpy(bytes + envelope.metadata_size + envelope.payload_size, envelope.digest, sizeof(envelope.digest));
    if (umi_cancellation_token_is_requested(cancel))
    {
        free(bytes);
        return UMI_STATUS_CANCELLED;
    }
    *out_bytes = bytes;
    *out_size = size;
    return UMI_STATUS_OK;
}
void UmiCreativeAssetArchiveFree(void *bytes) { free(bytes); }
UmiStatus UmiCreativeAssetArchiveDecodeBounded(const void *bytes, size_t size, size_t maximum_bytes,
                                               const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (maximum_bytes > UMI_CREATIVE_ASSET_MAX_BYTES || size > UMI_CREATIVE_ASSET_ARCHIVE_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (size < UMI_ASSET_ARCHIVE_HEADER_BYTES + UMI_NATIVE_SHA256_BYTES + 1U)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *wire = bytes;
    if (memcmp(wire, archive_magic, sizeof(archive_magic)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if (GetSize(wire + 8U, 2U) != 1U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (wire[11U] != 0U || wire[14U] != 0U || wire[15U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t declared = GetSize(wire + 16U, 8U);
    if (declared > maximum_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t label_size = (size_t)GetSize(wire + 12U, 2U), payload_size = (size_t)declared;
    if (label_size == 0U || label_size >= UMI_CREATIVE_LABEL_CAPACITY ||
        wire[10U] < (unsigned char)UMI_CREATIVE_ASSET_DOCUMENT ||
        wire[10U] > (unsigned char)UMI_CREATIVE_ASSET_BINARY)
        return UMI_STATUS_PARSE_ERROR;
    size_t payload_start = UMI_ASSET_ARCHIVE_HEADER_BYTES + label_size;
    if (size != payload_start + payload_size + UMI_NATIVE_SHA256_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    char label[UMI_CREATIVE_LABEL_CAPACITY];
    if (memchr(wire + UMI_ASSET_ARCHIVE_HEADER_BYTES, 0, label_size) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(label, wire + UMI_ASSET_ARCHIVE_HEADER_BYTES, label_size);
    label[label_size] = '\0';
    if (!UmiCreativeTextValid(label, sizeof(label), false))
        return UMI_STATUS_PARSE_ERROR;
    UmiNativeSha256 hash;
    unsigned char digest[UMI_NATIVE_SHA256_BYTES];
    UmiNativeSha256Init(&hash);
    UmiStatus status = HashBytes(&hash, wire, payload_start + payload_size, cancel);
    if (status == UMI_STATUS_OK)
        status = UmiNativeSha256Final(&hash, digest);
    if (status != UMI_STATUS_OK)
        return status;
    if (memcmp(digest, wire + payload_start + payload_size, sizeof(digest)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    /* Only validated complete input reaches the capture owner. Reuse its UTF-8
     * validation, memory ownership and cancellation-aware copy. Never restore
     * a machine path or treat stored purpose as media-format validation. */
    return UmiCreativeAssetCapture(label, (UmiCreativeAssetKind)wire[10U], wire + payload_start, payload_size,
                                   cancel, out);
}
UmiStatus UmiCreativeAssetArchiveDecode(const void *bytes, size_t size, const UmiCancellationToken *cancel,
                                        UmiCreativeAsset **out)
{
    return UmiCreativeAssetArchiveDecodeBounded(bytes, size, UMI_CREATIVE_ASSET_MAX_BYTES, cancel, out);
}
