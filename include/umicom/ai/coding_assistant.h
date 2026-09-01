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

typedef struct UmiAiCodingAssistantService UmiAiCodingAssistantService;

UmiStatus umi_ai_coding_assistant_create(
    UmiAiAuthorEngineService *authorengine,
    UmiAiCodingAssistantService **out_service);
void umi_ai_coding_assistant_destroy(UmiAiCodingAssistantService *service);
UmiAiCodingContextIndex *umi_ai_coding_assistant_context(
    UmiAiCodingAssistantService *service);
UmiStatus umi_ai_coding_assistant_set_patch_policy(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatchPolicy *policy);
UmiStatus umi_ai_coding_assistant_prepare(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingRequest *request,
    UmiAiProviderKind provider_kind,
    UmiAiCodingTaskPlan *out_plan);
UmiStatus umi_ai_coding_assistant_task_at(
    const UmiAiCodingAssistantService *service,
    size_t position,
    UmiAiCodingTaskPlan *out_plan);
UmiStatus umi_ai_coding_assistant_record_patch(
    UmiAiCodingAssistantService *service,
    const UmiAiCodingPatch *patch);
UmiStatus umi_ai_coding_assistant_find_patch(
    const UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiAiCodingPatch *out_patch);
UmiStatus umi_ai_coding_assistant_approve_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const char *approved_by);
UmiStatus umi_ai_coding_assistant_reject_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id);
UmiStatus umi_ai_coding_assistant_apply_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);
UmiStatus umi_ai_coding_assistant_revert_patch(
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    const UmiAiCodingFileAdapter *adapter);
UmiStatus umi_ai_coding_assistant_snapshot(
    const UmiAiCodingAssistantService *service,
    UmiAiCodingAssistantSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
