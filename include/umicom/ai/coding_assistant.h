/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_assistant.h
 *
 * PURPOSE:
 *   Coordinate code chat, completion, explanation, refactoring, test creation,
 *   repository context and controlled patch workflows through AuthorEngine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This service plans and governs coding work; it is not another model runtime.
 * AuthorEngine remains the orchestration authority, while products decide how
 * selected files are read and how approved patches reach their workspace.
 */
#ifndef UMICOM_AI_CODING_ASSISTANT_H
#define UMICOM_AI_CODING_ASSISTANT_H

#include "umicom/ai/authorengine_service.h"
#include "umicom/ai/coding_context.h"
#include "umicom/ai/coding_patch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding task plan data shared with callers of this public contract.
 */
typedef struct UmiAiCodingTaskPlan {
    UmiAiCodingRequest request;
    UmiAiCodingContextPlan repository_context;
    size_t general_context_sources;
    uint32_t general_context_tokens;
    uint32_t total_context_tokens;
    uint64_t plan_hash;
    uint64_t revision;
    int ready;
} UmiAiCodingTaskPlan;

/**
 * Represent the ai coding assistant snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingAssistantSnapshot {
    uint64_t revision;
    size_t repository_files;
    size_t task_count;
    size_t patch_count;
    size_t draft_patches;
    size_t approved_patches;
    size_t applied_patches;
    size_t reverted_patches;
    char last_request_id[UMI_AI_ID_CAPACITY];
    char last_patch_id[UMI_AI_ID_CAPACITY];
    UmiAiCodingPatchPolicy patch_policy;
} UmiAiCodingAssistantSnapshot;

/**
 * Represent the ai coding assistant service data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingAssistantService UmiAiCodingAssistantService;

/**
 * Initialise ai coding assistant from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_assistant_create(
    UmiAiAuthorEngineService *authorengine,
    UmiAiCodingAssistantService **out_service);
/**
 * Release or reset state held by ai coding assistant so the same storage can be reused
 * safely.
 */
void umi_ai_coding_assistant_destroy(UmiAiCodingAssistantService *service);
/**
 * Provide the ai coding assistant context operation used by this module and its client
 * applications.
 */
UmiAiCodingContextIndex *umi_ai_coding_assistant_context(
    UmiAiCodingAssistantService *service);
/**
 * Provide the ai coding assistant set patch policy operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_assistant_set_patch_policy(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatchPolicy *policy);
/**
 * Provide the ai coding assistant prepare operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_assistant_prepare(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingRequest *request,
    UmiAiProviderKind provider_kind,
    UmiAiCodingTaskPlan *out_plan);
/**
 * Find ai coding assistant task while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_coding_assistant_task_at(
    const UmiAiCodingAssistantService *service,
    size_t position,
    UmiAiCodingTaskPlan *out_plan);
/**
 * Provide the ai coding assistant record patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_assistant_record_patch(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatch *patch);
/**
 * Provide the ai coding assistant find patch operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_assistant_find_patch(
    const UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiAiCodingPatch *out_patch);
/**
 * Provide the ai coding assistant approve patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_assistant_approve_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const char *approved_by);
/**
 * Provide the ai coding assistant reject patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_assistant_reject_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id);
/**
 * Provide the ai coding assistant apply patch operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_assistant_apply_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);
/**
 * Provide the ai coding assistant revert patch operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_assistant_revert_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);
/**
 * Provide the ai coding assistant snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_assistant_snapshot(
    const UmiAiCodingAssistantService *service,
    UmiAiCodingAssistantSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
