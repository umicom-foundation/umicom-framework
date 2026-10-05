/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_archive_internal.h
 * PURPOSE: Share archive framing between portable memory codecs and native storage without exposing layout internals as public API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_ASSET_ARCHIVE_INTERNAL_H
#define UMICOM_CREATIVE_WORKSPACE_ASSET_ARCHIVE_INTERNAL_H
#include "umicom/creative_workspace/asset_archive.h"
#include "umicom/native_launcher/sha256.h"
#define UMI_ASSET_ARCHIVE_HEADER_BYTES 24U
typedef struct UmiCreativeAssetEnvelope
{
    unsigned char metadata[UMI_ASSET_ARCHIVE_HEADER_BYTES + UMI_CREATIVE_LABEL_CAPACITY - 1U];
    size_t metadata_size;
    const void *payload; /* Borrowed from the immutable asset. */
    size_t payload_size;
    unsigned char digest[UMI_NATIVE_SHA256_BYTES];
} UmiCreativeAssetEnvelope;
UmiStatus UmiCreativeAssetArchiveDescribe(const UmiCreativeAsset *asset, const UmiCancellationToken *cancel,
                                          UmiCreativeAssetEnvelope *out);
UmiStatus UmiCreativeAssetArchiveDecodeBounded(const void *bytes, size_t size, size_t maximum_bytes,
                                               const UmiCancellationToken *cancel, UmiCreativeAsset **out);
#endif
