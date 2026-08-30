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

/* BEGINNER NOTE:
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

typedef struct UmiAiCodingContextIndex UmiAiCodingContextIndex;

UmiStatus umi_ai_coding_context_index_create(
    UmiAiCodingContextIndex **out_index);
void umi_ai_coding_context_index_destroy(UmiAiCodingContextIndex *index);
UmiStatus umi_ai_coding_context_upsert(
    UmiAiCodingContextIndex *index,
    const UmiAiCodingContextFile *file);
UmiStatus umi_ai_coding_context_remove(UmiAiCodingContextIndex *index,
                                       const char *path);
void umi_ai_coding_context_clear(UmiAiCodingContextIndex *index);
size_t umi_ai_coding_context_count(const UmiAiCodingContextIndex *index);
UmiStatus umi_ai_coding_context_at(const UmiAiCodingContextIndex *index,
                                   size_t position,
                                   UmiAiCodingContextFile *out_file);
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
