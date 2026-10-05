/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/asset.h
 * PURPOSE: Capture complete local content for creative editors and provider adapters without coupling assets to a GUI or remote service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_ASSET_H
#define UMICOM_CREATIVE_WORKSPACE_ASSET_H
#include "umicom/creative_workspace/types.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CREATIVE_ASSET_MAX_BYTES (64U * 1024U * 1024U)
    /* Kind is the user's declared purpose, not the result of media validation.
 * Decoders and provider adapters must inspect the bytes before using them. */
    typedef enum UmiCreativeAssetKind
    {
        UMI_CREATIVE_ASSET_DOCUMENT = 1,
        UMI_CREATIVE_ASSET_IMAGE,
        UMI_CREATIVE_ASSET_VIDEO,
        UMI_CREATIVE_ASSET_AUDIO,
        UMI_CREATIVE_ASSET_LYRICS,
        UMI_CREATIVE_ASSET_BINARY
    } UmiCreativeAssetKind;
    typedef struct UmiCreativeAsset UmiCreativeAsset;
    typedef struct UmiCreativeAssetInfo
    {
        char label[UMI_CREATIVE_LABEL_CAPACITY];
        char source_path[UMI_PATH_CAPACITY]; /* Empty for supplied in-memory bytes. */
        UmiCreativeAssetKind declared_kind;
        size_t byte_count;
    } UmiCreativeAssetInfo;
    typedef struct UmiCreativeAssetWriteResult
    {
        bool created; /* A failure can still leave a new partial file for review. */
        UmiOutputFileSnapshot file;
    } UmiCreativeAssetWriteResult;
    /* Initialise *out=NULL. Success transfers one immutable asset; failure leaves
 * it unchanged, and an already-live output is refused. A complete UTF-8 label
 * and all binary bytes are copied. Empty assets are valid. No content is
 * decoded, executed, saved to a project/database or sent to a provider. */
    UmiStatus UmiCreativeAssetCapture(const char *label, UmiCreativeAssetKind kind, const void *bytes,
                                      size_t size, const UmiCancellationToken *cancel,
                                      UmiCreativeAsset **out);
    /* Capture one absolute regular local file, at most maximum_bytes and never
 * above MAX_BYTES. Final links/devices are refused by the shared file reader;
 * ancestors are trusted. Read on a worker. Cancellation is checked around the
 * blocking read, not inside OS I/O. Later disk changes cannot change the copy. */
    UmiStatus UmiCreativeAssetLoadFile(const char *path, const char *label, UmiCreativeAssetKind kind,
                                       size_t maximum_bytes, const UmiCancellationToken *cancel,
                                       UmiCreativeAsset **out);
    void UmiCreativeAssetDestroy(UmiCreativeAsset *asset);
    UmiStatus UmiCreativeAssetInspect(const UmiCreativeAsset *asset, UmiCreativeAssetInfo *out);
    /* Borrow the complete immutable bytes until Destroy. The caller must keep the
 * asset alive for every reader and must never write through this view. */
    UmiStatus UmiCreativeAssetBytes(const UmiCreativeAsset *asset, const void **out_bytes, size_t *out_size);
    /* Explicitly copy captured bytes to a new absolute file; never overwrite or
 * create parents. Writes belong on a worker. Cooperative cancellation between
 * chunks may leave a partial new file. Result reports creation and actual I/O;
 * the returned status reports cancellation/failure. All opened handles close.
 * Destination parents are trusted; storage is plaintext, not a secret vault. */
    UmiStatus UmiCreativeAssetWriteNew(const UmiCreativeAsset *asset, const char *path,
                                       const UmiCancellationToken *cancel, UmiCreativeAssetWriteResult *out);
#ifdef __cplusplus
}
#endif
#endif
