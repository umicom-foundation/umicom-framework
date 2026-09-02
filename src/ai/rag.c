/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/rag.c
 *
 * PURPOSE:
 *   Append ranked retrieval evidence to a prompt with source labels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each appended block keeps a stable chunk identifier so later UI layers can convert evidence into visible source attribution.
 */

#include "umicom/ai/rag.h"
#include <stddef.h>

#include <stdio.h>

/*
 * Provide the ai rag append results operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_rag_append_results(UmiAiPrompt *prompt,
                                    const UmiAiRetrievalResult *results,
                                    size_t count)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (prompt == NULL || (results == NULL && count != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        char header[256];
        int written = snprintf(header, sizeof(header),
                               "[source %s score %.3f]\n",
                               results[index].chunk.chunk_id,
                               results[index].score);
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(header)) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_ai_prompt_append(prompt, header);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_ai_prompt_append_line(prompt, results[index].chunk.text);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
