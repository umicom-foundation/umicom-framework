/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/asset_archive.h
 * PURPOSE: Save complete creative captures in a portable local archive without recording machine-specific source paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_ASSET_ARCHIVE_H
#define UMICOM_CREATIVE_WORKSPACE_ASSET_ARCHIVE_H
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* Fixed header, complete UTF-8 label and digest surround the captured bytes.
 * This limit includes the largest supported label, not just the payload. */
#define UMI_CREATIVE_ASSET_ARCHIVE_MAX_BYTES                                                                 \
    (UMI_CREATIVE_ASSET_MAX_BYTES + UMI_CREATIVE_LABEL_CAPACITY - 1U + 56U)
    /* Initialise *out_bytes=NULL. Encode publishes a complete owned archive only
 * on success; errors preserve both output arguments. Free with ArchiveFree.
 * The archive contains label, declared purpose and complete bytes. It omits
 * source paths, credentials and project membership. It is NOT encrypted.
 * Its SHA-256 digest detects corruption, not authorship or malicious changes. */
    UmiStatus UmiCreativeAssetArchiveEncode(const UmiCreativeAsset *asset, const UmiCancellationToken *cancel,
                                            unsigned char **out_bytes, size_t *out_size);
    void UmiCreativeAssetArchiveFree(void *bytes);
    /* Decode validates the complete envelope and digest before publishing a new
 * immutable capture. Initialise *out=NULL; a live output is refused. No path
 * in the file is opened and no content is decoded as media or executed.
 * Restored source_path is empty because a source path is not portable identity.
 * Cancellation is cooperative between bounded processing chunks. */
    UmiStatus UmiCreativeAssetArchiveDecode(const void *bytes, size_t size,
                                            const UmiCancellationToken *cancel, UmiCreativeAsset **out);
    /* Native storage uses the same portable encoding. Save creates a new absolute
 * file exclusively, without creating parents or overwriting an existing file.
 * A cancelled/failed save may leave a partial or complete new file; inspect
 * the returned receipt and choose another name before retrying. Parent paths
 * must be trusted. Handles close on every path. Run both operations on workers. */
    UmiStatus UmiCreativeAssetArchiveSaveNew(const UmiCreativeAsset *asset, const char *path,
                                             const UmiCancellationToken *cancel,
                                             UmiCreativeAssetWriteResult *out);
    /* Read at most the bounded archive size. maximum_bytes limits the restored
 * payload (0 permits only an empty capture). Native I/O is cancellable around
 * the blocking read; parsing and copying check cancellation between chunks.
 * Loading may temporarily hold the archive and restored capture together. */
    UmiStatus UmiCreativeAssetArchiveLoad(const char *path, size_t maximum_bytes,
                                          const UmiCancellationToken *cancel, UmiCreativeAsset **out);
#ifdef __cplusplus
}
#endif
#endif
