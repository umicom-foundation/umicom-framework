/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/asset_library_archive.h
 * PURPOSE: Save and reopen complete creative asset libraries without machine paths or external references.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_ASSET_LIBRARY_ARCHIVE_H
#define UMICOM_CREATIVE_WORKSPACE_ASSET_LIBRARY_ARCHIVE_H
#include "umicom/creative_workspace/asset_library.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* Envelope + title + each bounded ID and single-asset envelope + final digest. */
#define UMI_CREATIVE_LIBRARY_ARCHIVE_MAX_BYTES                                                               \
    (UMI_CREATIVE_LIBRARY_MAX_BYTES + 64U + UMI_CREATIVE_LABEL_CAPACITY - 1U +                               \
     UMI_CREATIVE_LIBRARY_MAX_ASSETS *                                                                       \
         (16U + UMI_CREATIVE_ID_CAPACITY - 1U + 56U + UMI_CREATIVE_LABEL_CAPACITY - 1U))
    /* Encoding is deterministic and plaintext. SHA-256 detects damaged data, not
 * trusted authorship. The bundle includes title, IDs, labels, declared purposes
 * and bytes, but no source paths, provider credentials or external file links.
 * Input owners must remain unchanged for the call. *out_bytes must be NULL;
 * failures preserve outputs. Release the result with ArchiveFree. */
    UmiStatus UmiCreativeAssetLibraryArchiveEncode(const UmiCreativeAssetLibrary *library,
                                                   const UmiCancellationToken *cancel,
                                                   unsigned char **out_bytes, size_t *out_size);
    void UmiCreativeAssetLibraryArchiveFree(void *bytes);
    /* Validate the whole envelope and every asset before publishing any library.
 * maximum_bytes bounds the combined payload (0 permits empty captures only).
 * A live *out is refused. Failed/cancelled decoding leaves it unchanged. */
    UmiStatus UmiCreativeAssetLibraryArchiveDecode(const void *bytes, size_t size, size_t maximum_bytes,
                                                   const UmiCancellationToken *cancel,
                                                   UmiCreativeAssetLibrary **out);
    /* Save streams owned assets to a new absolute file; it never overwrites, creates
 * parents or removes a failed partial file. The receipt records any new file.
 * Load reads a bounded complete archive then decodes; memory can include both
 * wire bytes and decoded captures. Retaining an old library adds its own cost.
 * Run native I/O on workers. Trust parent directories, not arbitrary ancestors.
 * Cancellation is cooperative between chunks and around blocking native reads. */
    UmiStatus UmiCreativeAssetLibrarySaveNew(const UmiCreativeAssetLibrary *library, const char *path,
                                             const UmiCancellationToken *cancel,
                                             UmiCreativeAssetWriteResult *out);
    UmiStatus UmiCreativeAssetLibraryLoad(const char *path, size_t maximum_bytes,
                                          const UmiCancellationToken *cancel, UmiCreativeAssetLibrary **out);
#ifdef __cplusplus
}
#endif
#endif
