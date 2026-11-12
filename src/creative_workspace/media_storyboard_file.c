/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_storyboard_file.c
 *
 * PURPOSE:
 *   Save and reopen explicit storyboard files through shared atomic-output and bounded-input services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/input_file.h"

UmiStatus UmiMediaStoryboardDraftSave(const UmiMediaStoryboardDraft *draft,
    const char *absolute_path, const UmiCancellationToken *cancel)
{
    UmiStatus status = UmiOutputFileValidatePath(absolute_path);
    UmiCreativeAsset *document = NULL;
    const void *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiMediaStoryboardDraftExport(draft, cancel, &document);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetBytes(document, &bytes, &size);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    /* Publication owns no cancellation rollback. Once it starts, return the
     * writer's actual outcome, not a later token state that could hide a save. */
    if (status == UMI_STATUS_OK)
        status = umi_atomic_file_write(absolute_path, bytes, size);
    UmiCreativeAssetDestroy(document);
    return status;
}
UmiStatus UmiMediaStoryboardDraftLoad(const char *absolute_path,
    const UmiCancellationToken *cancel, UmiMediaStoryboardDraft *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(absolute_path);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiInputFileRead(absolute_path, UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiMediaStoryboardDraftImport(bytes, size, cancel, out);
    UmiInputFileFree(bytes);
    return status;
}
