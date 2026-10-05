/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/hover_document.c
 * PURPOSE: Decode complete hover blocks and preserve literal text for safe presentation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/hover_document.h"
#include "umicom/language_runtime/response_tree.h"
#include <stdlib.h>
#include <string.h>
typedef struct HoverBlock
{
    UmiLanguageHoverContentKind kind;
    char language[128];
    char *text;
    size_t bytes;
} HoverBlock;
struct UmiLanguageHoverDocument
{
    HoverBlock *blocks;
    size_t count, text_bytes;
    char *text;
    int has_range;
    UmiEditorTextPosition start, end;
};
static UmiStatus HoverMember(const UmiJsonTree *tree, int object, const char *name, int required, int *out)
{
    *out = -1;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    if (status == UMI_STATUS_NOT_FOUND)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    return status;
}
static UmiStatus HoverText(const UmiJsonTree *tree, int node, char **out, size_t *bytes)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *raw = NULL;
    size_t length = 0U;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &raw, &length);
    (void)raw;
    if (status != UMI_STATUS_OK)
        return status;
    char *text = malloc(length + 1U);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, text, length + 1U);
    if (status != UMI_STATUS_OK)
        free(text);
    else
    {
        *bytes = strlen(text);
        *out = text;
    }
    return status;
}
static UmiStatus HoverBlockRead(const UmiJsonTree *tree, int node, int markup, HoverBlock *out)
{
    if (UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        out->kind = UMI_LANGUAGE_HOVER_MARKDOWN;
        return HoverText(tree, node, &out->text, &out->bytes);
    }
    int kind, language, value;
    UmiStatus status = HoverMember(tree, node, "kind", 0, &kind);
    if (status == UMI_STATUS_OK)
        status = HoverMember(tree, node, "language", 0, &language);
    if (status == UMI_STATUS_OK)
        status = HoverMember(tree, node, "value", 1, &value);
    if (status != UMI_STATUS_OK)
        return status;
    if ((kind < 0) == (language < 0) || (kind >= 0 && !markup))
        return UMI_STATUS_PARSE_ERROR;
    if (kind >= 0)
    {
        char name[32];
        status = UmiJsonTreeText(tree, kind, name, sizeof(name));
        if (status == UMI_STATUS_OK)
        {
            if (strcmp(name, "plaintext") == 0)
                out->kind = UMI_LANGUAGE_HOVER_PLAIN_TEXT;
            else if (strcmp(name, "markdown") == 0)
                out->kind = UMI_LANGUAGE_HOVER_MARKDOWN;
            else
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    else
    {
        out->kind = UMI_LANGUAGE_HOVER_CODE;
        status = UmiJsonTreeText(tree, language, out->language, sizeof(out->language));
    }
    if (status == UMI_STATUS_OK)
        status = HoverText(tree, value, &out->text, &out->bytes);
    return status;
}
static UmiStatus HoverPosition(const UmiJsonTree *tree, int node, UmiEditorTextPosition *out)
{
    int line, column;
    int64_t row = 0, col = 0;
    UmiStatus status = HoverMember(tree, node, "line", 1, &line);
    if (status == UMI_STATUS_OK)
        status = HoverMember(tree, node, "character", 1, &column);
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
static UmiStatus HoverRange(const UmiJsonTree *tree, int node, UmiLanguageHoverDocument *document)
{
    if (node < 0)
        return UMI_STATUS_OK;
    int start, end;
    UmiStatus status = HoverMember(tree, node, "start", 1, &start);
    if (status == UMI_STATUS_OK)
        status = HoverMember(tree, node, "end", 1, &end);
    if (status == UMI_STATUS_OK)
        status = HoverPosition(tree, start, &document->start);
    if (status == UMI_STATUS_OK)
        status = HoverPosition(tree, end, &document->end);
    if (status == UMI_STATUS_OK && (document->end.line < document->start.line ||
                                    (document->end.line == document->start.line &&
                                     document->end.utf16_column < document->start.utf16_column)))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        document->has_range = 1;
    return status;
}
UmiStatus UmiLanguageHoverDocumentCreate(const void *result_json, size_t bytes,
                                         const UmiCancellationToken *cancel,
                                         UmiLanguageHoverDocument **out_document)
{
    if (out_document == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_document = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 16384U, 16U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(result_json, bytes, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageHoverDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    int contents = -1, range = -1, array = 0;
    if (!UmiJsonTreeIsNull(tree, 0))
    {
        status = HoverMember(tree, 0, "contents", 1, &contents);
        if (status == UMI_STATUS_OK)
            status = HoverMember(tree, 0, "range", 0, &range);
        if (status == UMI_STATUS_OK)
            status = HoverRange(tree, range, document);
        if (status == UMI_STATUS_OK)
        {
            array = UmiJsonTreeKind(tree, contents) == UMI_LANGUAGE_RUNTIME_JSON_ARRAY;
            document->count = array ? UmiJsonTreeCount(tree, contents) : 1U;
            if (document->count > 256U)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    if (status == UMI_STATUS_OK && document->count != 0U)
    {
        document->blocks = calloc(document->count, sizeof(*document->blocks));
        if (document->blocks == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    int node = array ? UmiJsonTreeFirst(tree, contents) : contents;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < document->count;
         ++i, node = UmiJsonTreeNext(tree, node))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = HoverBlockRead(tree, node, !array, &document->blocks[i]);
        if (status == UMI_STATUS_OK)
            document->text_bytes += document->blocks[i].bytes + (i == 0U ? 0U : 2U);
    }
    if (status == UMI_STATUS_OK)
    {
        /* Decoded bytes are bounded by the input plus two separators per block.
         * Literal display preserves provider wording without evaluating markup. */
        document->text = malloc(document->text_bytes + 1U);
        if (document->text == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            size_t offset = 0U;
            for (size_t i = 0U; i < document->count; ++i)
            {
                if (i != 0U)
                {
                    memcpy(document->text + offset, "\n\n", 2U);
                    offset += 2U;
                }
                memcpy(document->text + offset, document->blocks[i].text, document->blocks[i].bytes);
                offset += document->blocks[i].bytes;
            }
            document->text[offset] = '\0';
        }
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status != UMI_STATUS_OK)
        UmiLanguageHoverDocumentDestroy(document);
    else
        *out_document = document;
    return status;
}
UmiStatus UmiLanguageHoverDocumentReadResponse(const void *response_json, size_t bytes,
                                               uint64_t expected_request_id,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageHoverDocument **out_document)
{
    if (out_document == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_document = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 16384U, 16U};
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(response_json, bytes, expected_request_id, &limits, cancel,
                                                   &tree, &result);
    const char *value = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &value, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageHoverDocumentCreate(value, length, cancel, out_document);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageHoverDocumentDestroy(UmiLanguageHoverDocument *document)
{
    if (document != NULL)
    {
        if (document->blocks != NULL)
            for (size_t i = 0U; i < document->count; ++i)
                free(document->blocks[i].text);
        free(document->blocks);
        free(document->text);
        free(document);
    }
}
size_t UmiLanguageHoverDocumentCount(const UmiLanguageHoverDocument *document)
{
    return document == NULL ? 0U : document->count;
}
UmiStatus UmiLanguageHoverDocumentAt(const UmiLanguageHoverDocument *document, size_t index,
                                     UmiLanguageHoverBlock *out_block)
{
    if (document == NULL || out_block == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= document->count)
        return UMI_STATUS_NOT_FOUND;
    const HoverBlock *block = &document->blocks[index];
    *out_block = (UmiLanguageHoverBlock){block->kind, block->language, block->text, block->bytes};
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageHoverDocumentText(const UmiLanguageHoverDocument *document, const char **out_text,
                                       size_t *out_bytes)
{
    if (document == NULL || out_text == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_text = document->text;
    *out_bytes = document->text_bytes;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageHoverDocumentRange(const UmiLanguageHoverDocument *document,
                                        UmiEditorTextPosition *out_start, UmiEditorTextPosition *out_end)
{
    if (document == NULL || out_start == NULL || out_end == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!document->has_range)
        return UMI_STATUS_NOT_FOUND;
    *out_start = document->start;
    *out_end = document->end;
    return UMI_STATUS_OK;
}
