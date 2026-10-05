/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/text_edit_preview.c
 * PURPOSE: Build reviewed whole-document results with exact coordinates and bounded owned text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/text_edit_preview.h"
#include "umicom/language_runtime/response_tree.h"
#include "umicom/editor/text_position_index.h"
#include <stdlib.h>
#include <string.h>
#define TEXT_EDIT_SOURCE_LIMIT (16U * 1024U * 1024U)
typedef struct TextEditSpan
{
    size_t begin, end, bytes, ordinal;
    char *text;
} TextEditSpan;
struct UmiLanguageTextEditPreview
{
    char *text;
    size_t bytes, caret, count;
};
static UmiStatus TextEditMember(const UmiJsonTree *tree, int object, const char *name, int *out)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus TextEditCoordinate(const UmiJsonTree *tree, int point, UmiEditorTextPosition *out)
{
    int line, column;
    int64_t row = 0, col = 0;
    UmiStatus status = TextEditMember(tree, point, "line", &line);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, point, "character", &column);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, line, &row);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, column, &col);
    if (status == UMI_STATUS_OK && (row < 0 || col < 0 || row > INT32_MAX || col > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = (UmiEditorTextPosition){(uint64_t)row, (uint64_t)col};
    return status;
}
/* Shared indexed coordinates avoid rescanning the complete source for every edit endpoint.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus TextEditRead(const UmiJsonTree *tree, int item, const UmiEditorTextBufferView *source,
                              size_t ordinal, TextEditSpan *out)
{
    if (UmiJsonTreeKind(tree, item) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    /* Annotated edits need a separate confirmation catalogue. Do not silently
     * accept an annotation identifier or an unknown alternate edit shape. */
    if (UmiJsonTreeCount(tree, item) != 2U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    int range, text, start, end;
    UmiStatus status = TextEditMember(tree, item, "range", &range);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, item, "newText", &text);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, range, "start", &start);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, range, "end", &end);
    UmiEditorTextPosition first, last;
    if (status == UMI_STATUS_OK)
        status = TextEditCoordinate(tree, start, &first);
    if (status == UMI_STATUS_OK)
        status = TextEditCoordinate(tree, end, &last);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewResolvePosition(source, first, &out->begin);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewResolvePosition(source, last, &out->end);
    if (status == UMI_STATUS_OK && out->end < out->begin)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, text) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        status = UMI_STATUS_PARSE_ERROR;
    const char *encoded = NULL;
    size_t encoded_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, text, &encoded, &encoded_bytes);
    (void)encoded;
    if (status == UMI_STATUS_OK)
    {
        /* Decoding JSON escapes never needs more bytes than the raw value.
         * Each allocation is bounded by the already bounded response tree. */
        out->text = malloc(encoded_bytes + 1U);
        if (out->text == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            status = UmiJsonTreeText(tree, text, out->text, encoded_bytes + 1U);
    }
    if (status == UMI_STATUS_OK)
    {
        out->bytes = strlen(out->text);
        out->ordinal = ordinal;
    }
    return status;
}
#endif
static UmiStatus TextEditRead(const UmiJsonTree *tree, int item, const UmiEditorTextPositionIndex *source,
                              size_t ordinal, TextEditSpan *out)
{
    if (UmiJsonTreeKind(tree, item) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    /* Annotated edits need a separate confirmation catalogue. Do not silently
     * accept an annotation identifier or an unknown alternate edit shape. */
    if (UmiJsonTreeCount(tree, item) != 2U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    int range, text, start, end;
    UmiStatus status = TextEditMember(tree, item, "range", &range);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, item, "newText", &text);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, range, "start", &start);
    if (status == UMI_STATUS_OK)
        status = TextEditMember(tree, range, "end", &end);
    UmiEditorTextPosition first, last;
    if (status == UMI_STATUS_OK)
        status = TextEditCoordinate(tree, start, &first);
    if (status == UMI_STATUS_OK)
        status = TextEditCoordinate(tree, end, &last);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(source, first, &out->begin);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(source, last, &out->end);
    if (status == UMI_STATUS_OK && out->end < out->begin)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, text) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        status = UMI_STATUS_PARSE_ERROR;
    const char *encoded = NULL;
    size_t encoded_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, text, &encoded, &encoded_bytes);
    (void)encoded;
    if (status == UMI_STATUS_OK)
    {
        /* Decoding JSON escapes never needs more bytes than the raw value.
         * Each allocation is bounded by the already bounded response tree. */
        out->text = malloc(encoded_bytes + 1U);
        if (out->text == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            status = UmiJsonTreeText(tree, text, out->text, encoded_bytes + 1U);
    }
    if (status == UMI_STATUS_OK)
    {
        out->bytes = strlen(out->text);
        out->ordinal = ordinal;
    }
    return status;
}
static int TextEditCompare(const void *left, const void *right)
{
    const TextEditSpan *a = left, *b = right;
    if (a->begin != b->begin)
        return a->begin < b->begin ? -1 : 1;
    return a->ordinal == b->ordinal ? 0 : (a->ordinal < b->ordinal ? -1 : 1);
}
/* Formatting owns one indexed source snapshot so repeated ranges remain fast and use identical captured bytes.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus TextEditBuild(const UmiJsonTree *tree, int root, const char *source, size_t source_bytes,
                               size_t caret, const UmiCancellationToken *cancel,
                               UmiLanguageTextEditPreview **out)
{
    if (source == NULL || caret > source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source_bytes > TEXT_EDIT_SOURCE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(source, '\0', source_bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = source_bytes;
    view.capacity = source_bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, source_bytes, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, caret, &position);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = 0U;
    if (!UmiJsonTreeIsNull(tree, root))
    {
        if (UmiJsonTreeKind(tree, root) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        count = UmiJsonTreeCount(tree, root);
    }
    if (count > 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    TextEditSpan *edits = calloc(count == 0U ? 1U : count, sizeof(*edits));
    if (edits == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    int item = UmiJsonTreeFirst(tree, root);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i, item = UmiJsonTreeNext(tree, item))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = TextEditRead(tree, item, &view, i, &edits[i]);
    }
    size_t result_bytes = source_bytes, consumed = 0U, removed = 0U, added = 0U;
    if (status == UMI_STATUS_OK)
        qsort(edits, count, sizeof(*edits), TextEditCompare);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        TextEditSpan *edit = &edits[i];
        if (edit->begin < consumed)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        consumed = edit->end;
        removed += edit->end - edit->begin;
        if (edit->bytes > TEXT_EDIT_SOURCE_LIMIT - added)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            added += edit->bytes;
    }
    /* Size the final transaction after all removals. An insertion near the
     * start may be offset by a later deletion; no intermediate text is built. */
    if (status == UMI_STATUS_OK)
    {
        result_bytes = source_bytes - removed;
        if (added > TEXT_EDIT_SOURCE_LIMIT - result_bytes)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            result_bytes += added;
    }
    UmiLanguageTextEditPreview *preview = NULL;
    if (status == UMI_STATUS_OK)
    {
        preview = calloc(1U, sizeof(*preview));
        if (preview == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            preview->text = malloc(result_bytes + 1U);
            if (preview->text == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    size_t at = 0U, used = 0U;
    int mapped = 0;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        TextEditSpan *edit = &edits[i];
        size_t prefix = edit->begin - at;
        if (!mapped && caret < edit->begin)
        {
            preview->caret = used + caret - at;
            mapped = 1;
        }
        memcpy(preview->text + used, source + at, prefix);
        used += prefix;
        if (!mapped && caret >= edit->begin && caret < edit->end)
        {
            preview->caret = used + edit->bytes;
            mapped = 1;
        }
        memcpy(preview->text + used, edit->text, edit->bytes);
        used += edit->bytes;
        at = edit->end;
    }
    if (status == UMI_STATUS_OK)
    {
        if (!mapped)
            preview->caret = used + caret - at;
        memcpy(preview->text + used, source + at, source_bytes - at);
        used += source_bytes - at;
        preview->text[used] = '\0';
        preview->bytes = used;
        preview->count = count;
        view.bytes = preview->text;
        view.byte_count = used;
        view.capacity = used;
        status = UmiEditorTextViewPositionAt(&view, preview->caret, &position);
        if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
    }
    for (size_t i = 0U; i < count; ++i)
        free(edits[i].text);
    free(edits);
    if (status != UMI_STATUS_OK)
        UmiLanguageTextEditPreviewDestroy(preview);
    else
        *out = preview;
    return status;
}
#endif
static UmiStatus TextEditBuild(const UmiJsonTree *tree, int root, const char *source, size_t source_bytes,
                               size_t caret, const UmiCancellationToken *cancel,
                               UmiLanguageTextEditPreview **out)
{
    if (source == NULL || caret > source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source_bytes > TEXT_EDIT_SOURCE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(source, '\0', source_bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = source_bytes;
    view.capacity = source_bytes;
    UmiEditorTextPosition position;
    UmiEditorTextPositionIndex *index = NULL;
    UmiStatus status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexAt(index, caret, &position);
    if (status != UMI_STATUS_OK)
    {
        UmiEditorTextPositionIndexDestroy(index);
        return status;
    }
    (void)UmiEditorTextPositionIndexView(index, &view);
    source = view.bytes;
    size_t count = 0U;
    if (!UmiJsonTreeIsNull(tree, root))
    {
        if (UmiJsonTreeKind(tree, root) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        {
            UmiEditorTextPositionIndexDestroy(index);
            return UMI_STATUS_PARSE_ERROR;
        }
        count = UmiJsonTreeCount(tree, root);
    }
    if (count > 256U)
    {
        UmiEditorTextPositionIndexDestroy(index);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    TextEditSpan *edits = calloc(count == 0U ? 1U : count, sizeof(*edits));
    if (edits == NULL)
    {
        UmiEditorTextPositionIndexDestroy(index);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    int item = UmiJsonTreeFirst(tree, root);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i, item = UmiJsonTreeNext(tree, item))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = TextEditRead(tree, item, index, i, &edits[i]);
    }
    size_t result_bytes = source_bytes, consumed = 0U, removed = 0U, added = 0U;
    if (status == UMI_STATUS_OK)
        qsort(edits, count, sizeof(*edits), TextEditCompare);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        TextEditSpan *edit = &edits[i];
        if (edit->begin < consumed)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        consumed = edit->end;
        removed += edit->end - edit->begin;
        if (edit->bytes > TEXT_EDIT_SOURCE_LIMIT - added)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            added += edit->bytes;
    }
    /* Size the final transaction after all removals. An insertion near the
     * start may be offset by a later deletion; no intermediate text is built. */
    if (status == UMI_STATUS_OK)
    {
        result_bytes = source_bytes - removed;
        if (added > TEXT_EDIT_SOURCE_LIMIT - result_bytes)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            result_bytes += added;
    }
    UmiLanguageTextEditPreview *preview = NULL;
    if (status == UMI_STATUS_OK)
    {
        preview = calloc(1U, sizeof(*preview));
        if (preview == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            preview->text = malloc(result_bytes + 1U);
            if (preview->text == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
        }
    }
    size_t at = 0U, used = 0U;
    int mapped = 0;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        TextEditSpan *edit = &edits[i];
        size_t prefix = edit->begin - at;
        if (!mapped && caret < edit->begin)
        {
            preview->caret = used + caret - at;
            mapped = 1;
        }
        memcpy(preview->text + used, source + at, prefix);
        used += prefix;
        if (!mapped && caret >= edit->begin && caret < edit->end)
        {
            preview->caret = used + edit->bytes;
            mapped = 1;
        }
        memcpy(preview->text + used, edit->text, edit->bytes);
        used += edit->bytes;
        at = edit->end;
    }
    if (status == UMI_STATUS_OK)
    {
        if (!mapped)
            preview->caret = used + caret - at;
        memcpy(preview->text + used, source + at, source_bytes - at);
        used += source_bytes - at;
        preview->text[used] = '\0';
        preview->bytes = used;
        preview->count = count;
        view.bytes = preview->text;
        view.byte_count = used;
        view.capacity = used;
        status = UmiEditorTextViewPositionAt(&view, preview->caret, &position);
        if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
    }
    for (size_t i = 0U; i < count; ++i)
        free(edits[i].text);
    free(edits);
    UmiEditorTextPositionIndexDestroy(index);
    if (status != UMI_STATUS_OK)
        UmiLanguageTextEditPreviewDestroy(preview);
    else
        *out = preview;
    return status;
}
UmiStatus UmiLanguageTextEditPreviewCreate(const void *result_json, size_t result_bytes, const char *source,
                                           size_t source_bytes, size_t caret,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageTextEditPreview **out_preview)
{
    if (out_preview == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 16384U, 16U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(result_json, result_bytes, &limits, cancel, &tree);
    if (status == UMI_STATUS_OK)
        status = TextEditBuild(tree, 0, source, source_bytes, caret, cancel, out_preview);
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus UmiLanguageTextEditPreviewReadResponse(const void *response_json, size_t response_bytes,
                                                 uint64_t expected_request_id, const char *source,
                                                 size_t source_bytes, size_t caret,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageTextEditPreview **out_preview)
{
    if (out_preview == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 16384U, 16U};
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(response_json, response_bytes, expected_request_id,
                                                   &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK)
        status = TextEditBuild(tree, result, source, source_bytes, caret, cancel, out_preview);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageTextEditPreviewDestroy(UmiLanguageTextEditPreview *preview)
{
    if (preview != NULL)
    {
        free(preview->text);
        free(preview);
    }
}
UmiStatus UmiLanguageTextEditPreviewRead(const UmiLanguageTextEditPreview *preview, const char **out_text,
                                         size_t *out_bytes, size_t *out_caret)
{
    if (preview == NULL || out_text == NULL || out_bytes == NULL || out_caret == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = preview->text;
    *out_bytes = preview->bytes;
    *out_caret = preview->caret;
    return UMI_STATUS_OK;
}
size_t UmiLanguageTextEditPreviewCount(const UmiLanguageTextEditPreview *preview)
{
    return preview == NULL ? 0U : preview->count;
}
