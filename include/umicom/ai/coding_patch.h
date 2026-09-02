/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_patch.h
 *
 * PURPOSE:
 *   Represent reviewable coding patches and apply or revert them through a
 *   conflict-aware, workspace-owned file adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the ai coding patch policy data shared with callers of this public contract.
 */
typedef struct UmiAiCodingPatchPolicy {
    size_t maximum_files;
    uint32_t maximum_changed_lines;
    int allow_create;
    int allow_delete;
    int require_approval;
} UmiAiCodingPatchPolicy;

/**
 * Represent the ai coding patch file data shared with callers of this public contract.
 */
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

/**
 * Represent the ai coding patch data shared with callers of this public contract.
 */
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

/**
 * Represent the ai coding file adapter data shared with callers of this public contract.
 */
typedef struct UmiAiCodingFileAdapter {
    uint32_t structure_size;
    uint32_t abi_version;
    UmiAiCodingFileRead read;
    UmiAiCodingFileWrite write;
    UmiAiCodingFileRemove remove;
    void *user_data;
} UmiAiCodingFileAdapter;

/**
 * Provide the ai coding patch policy default operation used by this module and its client
 * applications.
 */
UmiAiCodingPatchPolicy umi_ai_coding_patch_policy_default(void);
/**
 * Initialise ai coding patch from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_patch_init(UmiAiCodingPatch *patch,
                                   const char *patch_id,
                                   const char *request_id,
                                   const char *title,
                                   const char *rationale);
/**
 * Provide the ai coding patch add file operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_add_file(UmiAiCodingPatch *patch,
                                       const char *path,
                                       UmiAiCodingPatchOperation operation,
                                       const char *before_text,
                                       const char *after_text);
/**
 * Check that ai coding patch satisfies its contract before another service relies on it.
 */
UmiStatus umi_ai_coding_patch_validate(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingPatchPolicy *policy);
/**
 * Provide the ai coding patch approve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_approve(UmiAiCodingPatch *patch,
                                      const char *approved_by);
/**
 * Provide the ai coding patch reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_reject(UmiAiCodingPatch *patch);
/**
 * Perform ai coding patch through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_patch_apply(UmiAiCodingPatch *patch,
                                    const UmiAiCodingPatchPolicy *policy,
                                    const UmiAiCodingFileAdapter *adapter);
/**
 * Provide the ai coding patch revert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_patch_revert(UmiAiCodingPatch *patch,
                                     const UmiAiCodingFileAdapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
