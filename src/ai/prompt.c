/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/prompt.c
 *
 * PURPOSE:
 *   Append text safely to bounded AI prompts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The builder checks capacity before copying so a large prompt fails clearly instead of corrupting adjacent memory.
 */

#include "umicom/ai/prompt.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai prompt from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_prompt_init(UmiAiPrompt *prompt)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (prompt != NULL) {
        prompt->text[0] = '\0';
        prompt->length = 0U;
    }
}

/* Add ai prompt only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_prompt_append(UmiAiPrompt *prompt, const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (prompt == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (prompt->length + length + 1U > sizeof(prompt->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(prompt->text + prompt->length, text, length + 1U);
    prompt->length += length;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai prompt append line operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_prompt_append_line(UmiAiPrompt *prompt, const char *text)
{
    UmiStatus status = umi_ai_prompt_append(prompt, text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_ai_prompt_append(prompt, "\n");
}
