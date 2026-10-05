/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/format_plan.c
 * PURPOSE: Prepare bounded complete-text format conversions without changing documents or files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/format.h"
#include "umicom/document/line_endings.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>
struct UmiDocumentFormatPlan
{
    char *text;
    UmiDocumentFormatSummary summary;
};
/* Count output bytes before an original endpoint. Ordinary UTF-8 bytes are
 * copied unchanged; each complete ending consumes its original width and
 * contributes the requested width. Validation has already excluded split
 * characters and split CRLF pairs, so mapping cannot bisect either one. */
static size_t FormatMap(const char *source, size_t bytes, size_t endpoint, size_t width)
{
    size_t input = 0U, output = 0U;
    while (input < endpoint)
    {
        if (source[input] == '\r' || source[input] == '\n')
        {
            input += source[input] == '\r' && input + 1U < bytes && source[input + 1U] == '\n' ? 2U : 1U;
            output += width;
        }
        else
        {
            ++input;
            ++output;
        }
    }
    return output;
}
UmiStatus UmiDocumentFormatPlanCreate(const char *source, size_t bytes, size_t cursor, size_t selected,
                                      UmiDocumentLineEnding target, int final,
                                      const UmiCancellationToken *cancel, UmiDocumentFormatPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (source == NULL || target < UMI_DOCUMENT_LINE_ENDING_NONE || target > UMI_DOCUMENT_LINE_ENDING_CR ||
        (final != 0 && final != 1) || (target == UMI_DOCUMENT_LINE_ENDING_NONE && final))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bytes > UMI_DOCUMENT_FORMAT_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (cursor > bytes || selected > bytes - cursor)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (memchr(source, '\0', bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate the whole draft before mapping either endpoint. A valid caret
     * alone would not prove that later source bytes are valid Unicode. */
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = view.capacity = bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, cursor, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, cursor + selected, &position);
    UmiDocumentLineEndingAnalysis endings;
    if (status == UMI_STATUS_OK)
        status = umi_document_line_endings_analyse(source, bytes, &endings);
    if (status != UMI_STATUS_OK)
        return status;
    /* The input bound keeps this arithmetic small. Compute exact growth before
     * asking the existing converter to allocate or copy the complete draft. */
    size_t width = target == UMI_DOCUMENT_LINE_ENDING_CRLF ? 2U : 1U;
    size_t count = endings.lf_count + endings.crlf_count + endings.cr_count;
    size_t converted = target == UMI_DOCUMENT_LINE_ENDING_NONE ? bytes
                                                               : bytes - endings.lf_count - endings.cr_count -
                                                                     endings.crlf_count * 2U + count * width;
    int append = final && !endings.final_newline;
    if (append)
        converted += width;
    if (converted > UMI_DOCUMENT_FORMAT_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentFormatPlan *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t actual = 0U;
    if (target == UMI_DOCUMENT_LINE_ENDING_NONE)
    {
        plan->text = malloc(bytes + 1U);
        if (plan->text == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            memcpy(plan->text, source, bytes);
            plan->text[bytes] = '\0';
            actual = bytes;
        }
    }
    else
        status = umi_document_line_endings_normalise(source, bytes, target, final, &plan->text, &actual);
    if (status == UMI_STATUS_OK && actual != converted)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentFormatPlanDestroy(plan);
        return status;
    }
    plan->summary.source_bytes = bytes;
    plan->summary.proposed_bytes = actual;
    plan->summary.cursor_offset =
        target == UMI_DOCUMENT_LINE_ENDING_NONE ? cursor : FormatMap(source, bytes, cursor, width);
    size_t end = target == UMI_DOCUMENT_LINE_ENDING_NONE ? cursor + selected
                                                         : FormatMap(source, bytes, cursor + selected, width);
    plan->summary.selection_bytes = end - plan->summary.cursor_offset;
    plan->summary.replaced_endings =
        target == UMI_DOCUMENT_LINE_ENDING_NONE
            ? 0U
            : count - (target == UMI_DOCUMENT_LINE_ENDING_LF     ? endings.lf_count
                       : target == UMI_DOCUMENT_LINE_ENDING_CRLF ? endings.crlf_count
                                                                 : endings.cr_count);
    plan->summary.added_final_newline = append;
    plan->summary.text_changes = actual != bytes || memcmp(source, plan->text, bytes) != 0;
    *out = plan;
    return UMI_STATUS_OK;
}
void UmiDocumentFormatPlanDestroy(UmiDocumentFormatPlan *plan)
{
    if (plan != NULL)
    {
        free(plan->text);
        free(plan);
    }
}
UmiStatus UmiDocumentFormatPlanInspect(const UmiDocumentFormatPlan *plan, UmiDocumentFormatSummary *out)
{
    if (plan == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentFormatPlanRead(const UmiDocumentFormatPlan *plan, const char **out, size_t *bytes)
{
    if (out != NULL)
        *out = NULL;
    if (bytes != NULL)
        *bytes = 0U;
    if (plan == NULL || out == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->text;
    *bytes = plan->summary.proposed_bytes;
    return UMI_STATUS_OK;
}
