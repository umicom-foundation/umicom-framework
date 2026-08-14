/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_patch.h
 *
 * PURPOSE:
 *   Represent reviewable coding patches and apply or revert them through a
 *   conflict-aware, workspace-owned file adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The Framework never grants an AI direct filesystem access.  It first records
 * exact before/after text and hashes, then a product supplies narrow read,
 * write and remove callbacks after explicit approval.
 */
#ifndef UMICOM_AI_CODING_PATCH_H
#define UMICOM_AI_CODING_PATCH_H

#include "umicom/ai/coding_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingPatchPolicy {
    size_t maximum_files;
    uint32_t maximum_changed_lines;
    int allow_create;
    int allow_delete;
    int require_approval;
} UmiAiCodingPatchPolicy;

typedef struct UmiAiCodingPatchFile {
    char path[UMI_AI_TEXT_CAPACITY];
    UmiAiCodingPatchOperation operation;
    char before_text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    char after_text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t before_length;
    size_t after_length;
    uint64_t before_hash;
    uint64_t after_hash;
    uint32_t removed_lines;
    uint32_t added_lines;
} UmiAiCodingPatchFile;

typedef struct UmiAiCodingPatch {
    uint32_t structure_size;
    uint32_t abi_version;
    char patch_id[UMI_AI_ID_CAPACITY];
    char request_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char rationale[UMI_AI_TEXT_CAPACITY];
    char approved_by[UMI_AI_ID_CAPACITY];
    UmiAiCodingPatchState state;
    UmiAiCodingPatchFile files[UMI_AI_CODING_PATCH_FILE_MAX];
    size_t file_count;
    uint32_t changed_lines;
    uint64_t revision;
} UmiAiCodingPatch;

typedef UmiStatus (*UmiAiCodingFileRead)(
    void *user_data,
    const char *relative_path,
    char *out_text,
    size_t capacity,
    size_t *out_length);
typedef UmiStatus (*UmiAiCodingFileWrite)(
    void *user_data,
    const char *relative_path,
    const char *text,
    size_t length);
typedef UmiStatus (*UmiAiCodingFileRemove)(
    void *user_data,
    const char *relative_path);

typedef struct UmiAiCodingFileAdapter {
    uint32_t structure_size;
    uint32_t abi_version;
    UmiAiCodingFileRead read;
    UmiAiCodingFileWrite write;
    UmiAiCodingFileRemove remove;
    void *user_data;
} UmiAiCodingFileAdapter;

UmiAiCodingPatchPolicy umi_ai_coding_patch_policy_default(void);
UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale);
UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch,
                                       const char *path,
                                       UmiAiCodingPatchOperation operation,
                                       const char *before_text,
                                       const char *after_text);
UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy);
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch,
                                      const char *approved_by);
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch);
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter);
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
