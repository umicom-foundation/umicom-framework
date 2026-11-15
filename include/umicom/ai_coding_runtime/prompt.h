/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/prompt.h
 *
 * PURPOSE:
 *   Convert a governed coding task plan and materialized repository context into
 *   the existing provider-neutral UmiAiRequest message format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PROMPT_H
#define UMICOM_AI_CODING_RUNTIME_PROMPT_H

#include "umicom/ai_coding_runtime/config.h"
#include "umicom/ai_coding_runtime/context_materializer.h"
#include "umicom/ai_coding_runtime/prompt_template.h"
#include "umicom/ai_coding_runtime/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding build provider request operation used by this module and its
 * client applications.
 */
/* Both entry points validate the complete request, config and materialized
 * context, and require context provenance to match the selected plan. Every
 * refusal preserves out_request, including message-capacity failure. Inputs
 * may borrow the previous output but require live serialized storage.
 * Published request IDs, model IDs, message names and text must be valid UTF-8.
 * Malformed encoding is PARSE_ERROR. Chunking preserves scalar boundaries and
 * original bytes, without normalization or grapheme segmentation.
 * This checks data and provenance; it neither renews consent nor runs a model.
 * validation_feedback is NULL or a readable terminated C string. Strings that
 * cannot fit all message slots are rejected without an unbounded scan. */
UmiStatus umi_ai_coding_build_provider_request(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *validation_feedback,
    UmiAiRequest *out_request);

/* Length-aware feedback for adapters with byte buffers. feedback may be NULL
 * only for zero length; otherwise exactly feedback_length bytes must be
 * readable, with no embedded NUL. No trailing terminator is required. A length
 * above UMI_AI_MAX_MESSAGES * (UMI_AI_TEXT_CAPACITY - 1) is CAPACITY_EXCEEDED.
 * Actual available message slots may impose a smaller limit. */
UmiStatus UmiAiCodingBuildProviderRequestWithFeedback(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *feedback, size_t feedback_length,
    UmiAiRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
