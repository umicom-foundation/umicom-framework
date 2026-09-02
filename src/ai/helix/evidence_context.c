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

/*
 * Initialise ai helix evidence context from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_helix_evidence_context_init(UmiAiHelixEvidenceContext *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context != NULL) {
        memset(context, 0, sizeof(*context));
    }
}

/*
 * Add ai helix evidence context only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_helix_evidence_context_add(UmiAiHelixEvidenceContext *context,
                                            const char *label,
                                            const char *text)
{
    size_t label_length;
    size_t text_length;
    size_t required;
    char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || label == NULL || label[0] == '\0' || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    label_length = strlen(label);
    text_length = strlen(text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (label_length > SIZE_MAX - text_length - 5U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    required = label_length + text_length + 4U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
