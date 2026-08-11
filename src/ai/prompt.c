/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/prompt.c
 *
 * PURPOSE:
 *   Append text safely to bounded AI prompts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The builder checks capacity before copying so a large prompt fails clearly instead of corrupting adjacent memory.
 */

#include "umicom/ai/prompt.h"
#include <stddef.h>

#include <string.h>

void umi_ai_prompt_init(UmiAiPrompt *prompt)
{
    if (prompt != NULL) {
        prompt->text[0] = '\0';
        prompt->length = 0U;
    }
}

UmiStatus umi_ai_prompt_append(UmiAiPrompt *prompt, const char *text)
{
    size_t length;
    if (prompt == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (prompt->length + length + 1U > sizeof(prompt->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(prompt->text + prompt->length, text, length + 1U);
    prompt->length += length;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_prompt_append_line(UmiAiPrompt *prompt, const char *text)
{
    UmiStatus status = umi_ai_prompt_append(prompt, text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_ai_prompt_append(prompt, "\n");
}
