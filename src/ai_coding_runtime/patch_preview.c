/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/patch_preview.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral coding patch review projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/patch_preview.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_coding_patch_preview(
    const UmiAiCodingPatch *patch,
    UmiAiCodingPatchPreview *out_preview)
{
    size_t index;

    if (patch == NULL || out_preview == NULL ||
        patch->file_count > UMI_AI_CODING_PATCH_FILE_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_preview, 0, sizeof(*out_preview));
    (void)snprintf(
        out_preview->patch_id,
        sizeof(out_preview->patch_id),
        "%s",
        patch->patch_id);
    (void)snprintf(
        out_preview->title,
        sizeof(out_preview->title),
        "%s",
        patch->title);
    (void)snprintf(
        out_preview->rationale,
        sizeof(out_preview->rationale),
        "%s",
        patch->rationale);

    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *source = &patch->files[index];
        UmiAiCodingPatchPreviewFile *target =
            &out_preview->files[out_preview->file_count++];

        (void)snprintf(
            target->path,
            sizeof(target->path),
            "%s",
            source->path);
        target->operation = source->operation;
        target->added_lines = source->added_lines;
        target->removed_lines = source->removed_lines;
        target->before_length = source->before_length;
        target->after_length = source->after_length;

        out_preview->added_lines += source->added_lines;
        out_preview->removed_lines += source->removed_lines;

        switch (source->operation) {
            case UMI_AI_CODING_PATCH_CREATE:
                out_preview->create_count += 1U;
                break;
            case UMI_AI_CODING_PATCH_MODIFY:
                out_preview->modify_count += 1U;
                break;
            case UMI_AI_CODING_PATCH_DELETE:
                out_preview->delete_count += 1U;
                break;
            default:
                return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    out_preview->revision = patch->revision;
    return UMI_STATUS_OK;
}
