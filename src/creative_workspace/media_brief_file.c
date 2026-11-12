/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_brief_file.c
 *
 * PURPOSE:
 *   Publish and reopen explicit creation-draft files using Framework atomic output and bounded input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_brief_document.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/input_file.h"

UmiStatus UmiMediaBriefDocumentSave(const UmiMediaBriefDocument *document,
    const char *absolute_path, const UmiCancellationToken *cancel)
{
    UmiStatus status = UmiOutputFileValidatePath(absolute_path);
    UmiCreativeAsset *encoded = NULL;
    const void *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiMediaBriefDocumentExport(document, cancel, &encoded);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetBytes(encoded, &bytes, &size);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    /* Cancellation before publication prevents I/O. Once the atomic writer is
     * entered, report its outcome: a later cancellation cannot undo a rename. */
    if (status == UMI_STATUS_OK)
        status = umi_atomic_file_write(absolute_path, bytes, size);
    UmiCreativeAssetDestroy(encoded);
    return status;
}
UmiStatus UmiMediaBriefDocumentLoad(const char *absolute_path,
    const UmiCancellationToken *cancel, UmiMediaBriefDocument *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(absolute_path);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiInputFileRead(absolute_path, UMI_MEDIA_BRIEF_DOCUMENT_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiMediaBriefDocumentImport(bytes, size, cancel, out);
    UmiInputFileFree(bytes);
    return status;
}
