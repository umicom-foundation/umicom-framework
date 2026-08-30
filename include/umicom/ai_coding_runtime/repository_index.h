/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/repository_index.h
 *
 * PURPOSE:
 *   Populate the existing UmiAiCodingContextIndex from a real workspace scan so
 *   repository-aware AI context is Framework-owned rather than Studio-private.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_REPOSITORY_INDEX_H
#define UMICOM_AI_CODING_RUNTIME_REPOSITORY_INDEX_H

#include "umicom/ai_coding_runtime/language.h"
#include "umicom/ai_coding_runtime/scanner.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingRepositoryIndexSnapshot {
    size_t scanned_files;
    size_t indexed_files;
    size_t ignored_non_text_files;
    size_t capacity_dropped_files;
    uint64_t revision;
    int truncated;
} UmiAiCodingRepositoryIndexSnapshot;

typedef struct UmiAiCodingRepositoryIndex UmiAiCodingRepositoryIndex;

UmiStatus umi_ai_coding_repository_index_create(
    UmiAiCodingContextIndex *context_index,
    UmiAiCodingRepositoryIndex **out_index);

void umi_ai_coding_repository_index_destroy(
    UmiAiCodingRepositoryIndex *index);

UmiStatus umi_ai_coding_repository_index_refresh(
    UmiAiCodingRepositoryIndex *index,
    const char *workspace_root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiDataClassification classification);

UmiStatus umi_ai_coding_repository_index_snapshot(
    const UmiAiCodingRepositoryIndex *index,
    UmiAiCodingRepositoryIndexSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
