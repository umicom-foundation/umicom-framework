/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/context_summary.c
 *
 * PURPOSE:
 *   Format repository context provenance without source content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/context_summary.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_coding_context_summary(
    const UmiAiCodingMaterializedContext *context,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;

    if (context == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        capacity,
        "Context files: %zu, skipped: %zu, estimated tokens: %u%s\n",
        context->file_count,
        context->skipped_count,
        context->estimated_tokens,
        context->truncated ? ", truncated" : "");

    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    for (index = 0U; index < context->file_count; ++index) {
        const UmiAiCodingMaterializedFile *file = &context->files[index];

        written = snprintf(
            out_text + used,
            capacity - used,
            "%s%s [%s] %zu bytes hash=%016llx\n",
            file->active ? "* " : "  ",
            file->path,
            file->language_id,
            file->length,
            (unsigned long long)file->hash);

        if (written < 0 || (size_t)written >= capacity - used) {
            out_text[capacity - 1U] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}
