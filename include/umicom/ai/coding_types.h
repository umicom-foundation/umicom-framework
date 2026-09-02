/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_types.h
 *
 * PURPOSE:
 *   Define the stable C23 vocabulary shared by AI coding-assistant services,
 *   repository adapters, patch reviewers and product frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These are owned, bounded values rather than pointers into an editor.  That
 * keeps the public ABI predictable and lets desktop, web and headless products
 * use exactly the same request and audit records.
 */
#ifndef UMICOM_AI_CODING_TYPES_H
#define UMICOM_AI_CODING_TYPES_H

#include "umicom/ai/privacy_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_ABI_VERSION 1U
#define UMI_AI_CODING_CONTEXT_FILE_CAPACITY 128U
#define UMI_AI_CODING_CONTEXT_PLAN_MAX 32U
#define UMI_AI_CODING_PATCH_FILE_MAX 16U
#define UMI_AI_CODING_PATCH_CONTENT_CAPACITY 8192U
#define UMI_AI_CODING_TASK_HISTORY_CAPACITY 32U
#define UMI_AI_CODING_PATCH_HISTORY_CAPACITY 16U

/**
 * List the named ai coding task kind values accepted by this public contract.
 */
typedef enum UmiAiCodingTaskKind {
    UMI_AI_CODING_TASK_CHAT = 1,
    UMI_AI_CODING_TASK_COMPLETE = 2,
    UMI_AI_CODING_TASK_EXPLAIN = 3,
    UMI_AI_CODING_TASK_REFACTOR = 4,
    UMI_AI_CODING_TASK_GENERATE_TESTS = 5
} UmiAiCodingTaskKind;

/**
 * List the named ai coding patch operation values accepted by this public contract.
 */
typedef enum UmiAiCodingPatchOperation {
    UMI_AI_CODING_PATCH_CREATE = 1,
    UMI_AI_CODING_PATCH_MODIFY = 2,
    UMI_AI_CODING_PATCH_DELETE = 3
} UmiAiCodingPatchOperation;

/**
 * List the named ai coding patch state values accepted by this public contract.
 */
typedef enum UmiAiCodingPatchState {
    UMI_AI_CODING_PATCH_DRAFT = 1,
    UMI_AI_CODING_PATCH_APPROVED = 2,
    UMI_AI_CODING_PATCH_APPLIED = 3,
    UMI_AI_CODING_PATCH_REVERTED = 4,
    UMI_AI_CODING_PATCH_REJECTED = 5
} UmiAiCodingPatchState;

/**
 * Represent the ai coding request data shared with callers of this public contract.
 */
typedef struct UmiAiCodingRequest {
    uint32_t structure_size;
    uint32_t abi_version;
    UmiAiCodingTaskKind task;
    UmiAiDataClassification classification;
    char request_id[UMI_AI_ID_CAPACITY];
    char session_id[UMI_AI_ID_CAPACITY];
    char runtime_id[UMI_AI_ID_CAPACITY];
    char workspace_root[UMI_AI_TEXT_CAPACITY];
    char active_path[UMI_AI_TEXT_CAPACITY];
    char language_id[UMI_AI_ID_CAPACITY];
    char instruction[UMI_AI_TEXT_CAPACITY];
    uint32_t selection_start_line;
    uint32_t selection_end_line;
    uint32_t context_token_budget;
    size_t maximum_context_files;
    uint64_t timestamp_ns;
    int sensitive_approved;
} UmiAiCodingRequest;

/**
 * Initialise ai coding request from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_coding_request_init(UmiAiCodingRequest *request,
                                UmiAiCodingTaskKind task);
/**
 * Check that ai coding request satisfies its contract before another service relies on it.
 */
UmiStatus umi_ai_coding_request_validate(const UmiAiCodingRequest *request);
/**
 * Provide the ai coding path is safe relative operation used by this module and its client
 * applications.
 */
int umi_ai_coding_path_is_safe_relative(const char *path);
/**
 * Provide the ai coding text hash operation used by this module and its client
 * applications.
 */
uint64_t umi_ai_coding_text_hash(const char *text, size_t length);
/**
 * Provide the ai coding task kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_task_kind_text(UmiAiCodingTaskKind task);
/**
 * Provide the ai coding patch operation text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_patch_operation_text(
    UmiAiCodingPatchOperation operation);
/**
 * Provide the ai coding patch state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_patch_state_text(UmiAiCodingPatchState state);

#ifdef __cplusplus
}
#endif

#endif
