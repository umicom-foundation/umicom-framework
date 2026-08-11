/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/prompt.h
 *
 * PURPOSE:
 *   Build bounded prompt text from several deterministic fragments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiAiPrompt {
    char text[UMI_AI_TEXT_CAPACITY * 4U];
    size_t length;
} UmiAiPrompt;

void umi_ai_prompt_init(UmiAiPrompt *prompt);
UmiStatus umi_ai_prompt_append(UmiAiPrompt *prompt, const char *text);
UmiStatus umi_ai_prompt_append_line(UmiAiPrompt *prompt, const char *text);

#ifdef __cplusplus
}
#endif

#endif
