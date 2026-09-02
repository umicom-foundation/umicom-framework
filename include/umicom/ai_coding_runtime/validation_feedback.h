/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_feedback.h
 *
 * PURPOSE:
 *   Convert failed validation evidence into a bounded repair prompt for the next
 *   agent iteration without losing exact command output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_FEEDBACK_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_FEEDBACK_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding validation feedback operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_validation_feedback(
    const UmiAiCodingValidationReport *report,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
