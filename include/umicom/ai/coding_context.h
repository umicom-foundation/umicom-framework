/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/coding_context.h
 *
 * PURPOSE:
 *   Index repository file metadata and build explainable, privacy-aware coding
 *   context plans within explicit file and token budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The index stores provenance and cost estimates, not an IDE's file objects.
 * A product adapter remains responsible for reading the selected files only
 * after the user and provider policies have accepted the plan.
 */
#ifndef UMICOM_AI_CODING_CONTEXT_H
#define UMICOM_AI_CODING_CONTEXT_H

#include "umicom/ai/coding_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding context file data shared with callers of this public contract.
 */
typedef struct UmiAiCodingContextFile {
    char path[UMI_AI_TEXT_CAPACITY];
    char language_id[UMI_AI_ID_CAPACITY];
    char summary[UMI_AI_SMALL_TEXT_CAPACITY];
    UmiAiDataClassification classification;
    uint32_t estimated_tokens;
    uint32_t priority;
    uint64_t revision;
    int active;
    int enabled;
} UmiAiCodingContextFile;

/**
 * Represent the ai coding context plan data shared with callers of this public contract.
 */
typedef struct UmiAiCodingContextPlan {
    UmiAiCodingContextFile files[UMI_AI_CODING_CONTEXT_PLAN_MAX];
    size_t file_count;
    size_t dropped_count;
    uint32_t token_budget;
    uint32_t selected_tokens;
    uint32_t remaining_tokens;
    UmiAiDataClassification highest_classification;
    uint64_t revision;
    int truncated;
} UmiAiCodingContextPlan;

/**
 * Represent the ai coding context index data shared with callers of this public contract.
 */
typedef struct UmiAiCodingContextIndex UmiAiCodingContextIndex;

/**
 * Initialise ai coding context index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_context_index_create(
    UmiAiCodingContextIndex **out_index);
/**
 * Release or reset state held by ai coding context index so the same storage can be reused
 * safely.
 */
void umi_ai_coding_context_index_destroy(UmiAiCodingContextIndex *index);
/**
 * Provide the ai coding context upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_context_upsert(
    UmiAiCodingContextIndex *index,
    const UmiAiCodingContextFile *file);
/**
 * Remove ai coding context while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ai_coding_context_remove(UmiAiCodingContextIndex *index,
                                       const char *path);
/**
 * Release or reset state held by ai coding context so the same storage can be reused
 * safely.
 */
void umi_ai_coding_context_clear(UmiAiCodingContextIndex *index);
/**
 * Return the number of records represented by ai coding context without changing their
 * state.
 */
size_t umi_ai_coding_context_count(const UmiAiCodingContextIndex *index);
/**
 * Find ai coding context while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ai_coding_context_at(const UmiAiCodingContextIndex *index,
                                   size_t position,
                                   UmiAiCodingContextFile *out_file);
/**
 * Provide the ai coding context plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_context_plan(
    const UmiAiCodingContextIndex *index,
    uint32_t token_budget,
    size_t maximum_files,
    UmiAiProviderKind provider_kind,
    const UmiAiPrivacyPolicy *privacy,
    int sensitive_approved,
    UmiAiCodingContextPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
