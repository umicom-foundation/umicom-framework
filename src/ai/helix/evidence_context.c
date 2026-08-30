/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/evidence_context.c
 *
 * PURPOSE:
 *   Append labelled evidence atomically into a bounded bridge context buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/evidence_context.h"
#include <string.h>

void umi_ai_helix_evidence_context_init(UmiAiHelixEvidenceContext *context)
{
    if (context != NULL) {
        memset(context, 0, sizeof(*context));
    }
}

UmiStatus umi_ai_helix_evidence_context_add(UmiAiHelixEvidenceContext *context,
                                            const char *label,
                                            const char *text)
{
    size_t label_length;
    size_t text_length;
    size_t required;
    char *cursor;
    if (context == NULL || label == NULL || label[0] == '\0' || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    label_length = strlen(label);
    text_length = strlen(text);
    if (label_length > SIZE_MAX - text_length - 5U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    required = label_length + text_length + 4U;
    if (context->length > sizeof(context->text) - 1U ||
        required > sizeof(context->text) - 1U - context->length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    cursor = context->text + context->length;
    *cursor++ = '[';
    memcpy(cursor, label, label_length);
    cursor += label_length;
    *cursor++ = ']';
    *cursor++ = '\n';
    memcpy(cursor, text, text_length);
    cursor += text_length;
    *cursor++ = '\n';
    *cursor = '\0';
    context->length += required;
    ++context->item_count;
    return UMI_STATUS_OK;
}
