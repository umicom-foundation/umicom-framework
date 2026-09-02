/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/checkpoint.h
 *
 * PURPOSE:
 *   Capture bounded complete-file workspace checkpoints before multi-step agent
 *   mutations and restore them through the same controlled workspace adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CHECKPOINT_H
#define UMICOM_AI_CODING_TOOLS_CHECKPOINT_H

#include "umicom/ai_coding_runtime/workspace.h"
#include "umicom/ai_coding_tools/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding checkpoint file data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingCheckpointFile {
    char path[UMI_AI_TEXT_CAPACITY];
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length;
    uint64_t hash;
    int existed;
} UmiAiCodingCheckpointFile;

/**
 * Represent the ai coding checkpoint data shared with callers of this public contract.
 */
typedef struct UmiAiCodingCheckpoint {
    char checkpoint_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char label[256];
    UmiAiCodingCheckpointFile
        files[UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY];
    size_t file_count;
    uint64_t revision;
} UmiAiCodingCheckpoint;

/**
 * Represent the ai coding checkpoint store data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingCheckpointStore {
    UmiAiCodingCheckpoint checkpoints[UMI_AI_CODING_TOOL_CHECKPOINT_CAPACITY];
    size_t count;
    uint64_t sequence;
} UmiAiCodingCheckpointStore;

/**
 * Initialise ai coding checkpoint store from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_checkpoint_store_init(UmiAiCodingCheckpointStore *store);

/**
 * Provide the ai coding checkpoint capture operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_checkpoint_capture(
    UmiAiCodingCheckpointStore *store,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *checkpoint_id,
    const char *label,
    const char *const *paths,
    size_t path_count);

/**
 * Find ai coding checkpoint while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_checkpoint_find(
    const UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id,
    UmiAiCodingCheckpoint *out_checkpoint);

/**
 * Provide the ai coding checkpoint restore operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_checkpoint_restore(
    UmiAiCodingCheckpointStore *store,
    const UmiAiCodingWorkspaceAdapter *workspace,
    const char *checkpoint_id);

/**
 * Remove ai coding checkpoint while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_coding_checkpoint_remove(
    UmiAiCodingCheckpointStore *store,
    const char *checkpoint_id);

#ifdef __cplusplus
}
#endif
#endif
