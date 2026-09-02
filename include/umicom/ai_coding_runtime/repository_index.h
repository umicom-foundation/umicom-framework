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

/**
 * Represent the ai coding repository index snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiAiCodingRepositoryIndexSnapshot {
    size_t scanned_files;
    size_t indexed_files;
    size_t ignored_non_text_files;
    size_t capacity_dropped_files;
    uint64_t revision;
    int truncated;
} UmiAiCodingRepositoryIndexSnapshot;

/**
 * Represent the ai coding repository index data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingRepositoryIndex UmiAiCodingRepositoryIndex;

/**
 * Initialise ai coding repository index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_coding_repository_index_create(
    UmiAiCodingContextIndex *context_index,
    UmiAiCodingRepositoryIndex **out_index);

/**
 * Release or reset state held by ai coding repository index so the same storage can be
 * reused safely.
 */
void umi_ai_coding_repository_index_destroy(
    UmiAiCodingRepositoryIndex *index);

/**
 * Provide the ai coding repository index refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_repository_index_refresh(
    UmiAiCodingRepositoryIndex *index,
    const char *workspace_root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiDataClassification classification);

/**
 * Provide the ai coding repository index snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_repository_index_snapshot(
    const UmiAiCodingRepositoryIndex *index,
    UmiAiCodingRepositoryIndexSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
