/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/rag.c
 *
 * PURPOSE:
 *   Append ranked retrieval evidence to a prompt with source labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each appended block keeps a stable chunk identifier so later UI layers can convert evidence into visible source attribution.
 */

#include "umicom/ai/rag.h"
#include <stddef.h>

#include <stdio.h>

UmiStatus umi_ai_rag_append_results(UmiAiPrompt *prompt,
                                    const UmiAiRetrievalResult *results,
                                    size_t count)
{
    size_t index;
    if (prompt == NULL || (results == NULL && count != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < count; ++index) {
        char header[256];
        int written = snprintf(header, sizeof(header),
                               "[source %s score %.3f]\n",
                               results[index].chunk.chunk_id,
                               results[index].score);
        UmiStatus status;
        if (written < 0 || (size_t)written >= sizeof(header)) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_ai_prompt_append(prompt, header);
        if (status != UMI_STATUS_OK) return status;
        status = umi_ai_prompt_append_line(prompt, results[index].chunk.text);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
