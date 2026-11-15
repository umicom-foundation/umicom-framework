/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/context_summary.h
 *
 * PURPOSE:
 *   Produce a compact explainable summary of materialized repository context for
 *   UI inspection, audit and debugging without copying full source text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_CONTEXT_SUMMARY_H
#define UMICOM_AI_CODING_RUNTIME_CONTEXT_SUMMARY_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding context summary operation used by this module and its client
 * applications.
 */
/* Format metadata without reading source text or certifying its displayed
 * hash, length or token estimates. Count and label bounds are checked before
 * traversal. Paths are relative; labels are UTF-8 without ASCII control bytes.
 * Preserve out_text on every failure, including insufficient capacity. The
 * destination may overlap input metadata when its actual writable capacity
 * is sufficient; publication follows all input reads. Serialize access. */
UmiStatus umi_ai_coding_context_summary(
    const UmiAiCodingMaterializedContext *context,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
