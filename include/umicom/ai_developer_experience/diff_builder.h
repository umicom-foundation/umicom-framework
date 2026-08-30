/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/diff_builder.h
 *
 * PURPOSE:
 *   Build deterministic line-level diffs from complete before/after source text.
 *
 * IMPLEMENTATION:
 *   A bounded LCS dynamic-programming matrix is allocated on the heap. This
 *   avoids large thread-stack allocations while remaining deterministic for the
 *   complete-file patch sizes used by the governed coding assistant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_BUILDER_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_DIFF_BUILDER_H
#include "umicom/ai_developer_experience/diff.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_diff_build(
    const char *path,
    UmiAiCodingPatchOperation operation,
    const char *before_text,
    const char *after_text,
    uint64_t before_hash,
    uint64_t after_hash,
    UmiAiDeveloperFileDiff *out_diff);

#ifdef __cplusplus
}
#endif
#endif
