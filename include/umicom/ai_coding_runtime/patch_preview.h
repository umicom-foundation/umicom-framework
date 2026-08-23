/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/patch_preview.h
 *
 * PURPOSE:
 *   Project a governed complete-file coding patch into a compact review model
 *   that any IDE, web UI or headless audit tool can render before approval.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PATCH_PREVIEW_H
#define UMICOM_AI_CODING_RUNTIME_PATCH_PREVIEW_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingPatchPreviewFile {
    char path[UMI_AI_TEXT_CAPACITY];
    UmiAiCodingPatchOperation operation;
    uint32_t added_lines;
    uint32_t removed_lines;
    size_t before_length;
    size_t after_length;
} UmiAiCodingPatchPreviewFile;

typedef struct UmiAiCodingPatchPreview {
    char patch_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    UmiAiCodingPatchPreviewFile files[UMI_AI_CODING_PATCH_FILE_MAX];
    size_t file_count;
    size_t create_count;
    size_t modify_count;
    size_t delete_count;
    uint32_t added_lines;
    uint32_t removed_lines;
    uint64_t revision;
} UmiAiCodingPatchPreview;

UmiStatus umi_ai_coding_patch_preview(
    const UmiAiCodingPatch *patch,
    UmiAiCodingPatchPreview *out_preview);

#ifdef __cplusplus
}
#endif
#endif
