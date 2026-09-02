/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/prompt.h
 *
 * PURPOSE:
 *   Build bounded prompt text from several deterministic fragments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A bounded builder makes prompt construction inspectable and avoids hidden heap allocations for ordinary Studio requests.
 */

#ifndef INCLUDE_UMICOM_AI_PROMPT_H
#define INCLUDE_UMICOM_AI_PROMPT_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/ai/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai prompt data shared with callers of this public contract.
 */
typedef struct UmiAiPrompt {
    char text[UMI_AI_TEXT_CAPACITY * 4U];
    size_t length;
} UmiAiPrompt;

/**
 * Initialise ai prompt from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_prompt_init(UmiAiPrompt *prompt);
/**
 * Add ai prompt only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_prompt_append(UmiAiPrompt *prompt, const char *text);
/**
 * Provide the ai prompt append line operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_prompt_append_line(UmiAiPrompt *prompt, const char *text);

#ifdef __cplusplus
}
#endif

#endif
