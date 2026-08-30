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

UmiStatus umi_ai_coding_build_provider_request(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingMaterializedContext *context,
    const UmiAiCodingRuntimeConfig *config,
    const char *validation_feedback,
    UmiAiRequest *out_request);

#ifdef __cplusplus
}
#endif
#endif
