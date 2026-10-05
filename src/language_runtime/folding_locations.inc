/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/folding_locations.inc
 * PURPOSE: Validate complete folding regions against captured source before publishing owned navigation rows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_position_index.h"

static UmiStatus FoldingLine(const UmiJsonTree *tree, int node, const char *name, int required, uint64_t *out)
{
    int member = -1;
    int64_t value = 0;
    UmiStatus status = LocationMember(tree, node, name, required, &member);
    if (status == UMI_STATUS_OK && member >= 0)
        status = UmiJsonTreeInteger(tree, member, &value);
    if (status == UMI_STATUS_OK && (value < 0 || value > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = (uint64_t)value;
    return status;
}
static UmiStatus FoldingOptionalText(const UmiJsonTree *tree, int node, const char *name)
{
    int member = -1;
    UmiStatus status = LocationMember(tree, node, name, 0, &member);
    if (status == UMI_STATUS_OK && member >= 0 &&
        UmiJsonTreeKind(tree, member) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
UmiStatus UmiLanguageLocationCatalogueReadFoldingResponse(const void *json, size_t bytes,
                                                          uint64_t expected_id, const char *uri,
                                                          const char *source, size_t source_bytes,
                                                          const UmiCancellationToken *cancel,
                                                          UmiLanguageLocationCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (uri == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source_bytes > 16U * 1024U * 1024U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(source, '\0', source_bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t uri_bytes = strlen(uri);
    UmiStatus status = LocationUriValidate(uri, uri_bytes);
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    if (status == UMI_STATUS_OK)
        status = UmiLanguageResponseTreeRead(json, bytes, expected_id, &limits, cancel, &tree, &result);
    size_t count = 0U;
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(tree, result))
    {
        if (UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
            count = UmiJsonTreeCount(tree, result);
        if (count > 4096U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* One immutable position index validates Unicode and line endings once.
     * Never clamp a remote line to a different local line or publish a prefix
     * of a malformed response. No returned row borrows the source buffer. */
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = view.capacity = source_bytes;
    UmiEditorTextPositionIndex *index = NULL;
    UmiEditorTextPosition source_end = {0};
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexAt(index, source_bytes, &source_end);
    UmiLanguageLocationCatalogue *catalogue = NULL;
    if (status == UMI_STATUS_OK)
    {
        catalogue = calloc(1U, sizeof(*catalogue));
        if (catalogue == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else if (count != 0U)
        {
            catalogue->entries = calloc(count, sizeof(*catalogue->entries));
            if (catalogue->entries == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
            else
                catalogue->count = count;
        }
    }
    int node = status == UMI_STATUS_OK ? UmiJsonTreeFirst(tree, result) : -1;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i, node = UmiJsonTreeNext(tree, node))
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        uint64_t first = 0U, last = 0U, ignored = 0U;
        status = FoldingLine(tree, node, "startLine", 1, &first);
        if (status == UMI_STATUS_OK)
            status = FoldingLine(tree, node, "endLine", 1, &last);
        if (status == UMI_STATUS_OK)
            status = FoldingLine(tree, node, "startCharacter", 0, &ignored);
        if (status == UMI_STATUS_OK)
            status = FoldingLine(tree, node, "endCharacter", 0, &ignored);
        if (status == UMI_STATUS_OK)
            status = FoldingOptionalText(tree, node, "kind");
        if (status == UMI_STATUS_OK)
            status = FoldingOptionalText(tree, node, "collapsedText");
        if (status == UMI_STATUS_OK && first > last)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && last > source_end.line)
            status = UMI_STATUS_INVALID_ARGUMENT;
        UmiLanguageSourceRange range = {
            {first, 0U}, last < source_end.line ? (UmiEditorTextPosition){last + 1U, 0U} : source_end};
        size_t offset = 0U;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextPositionIndexResolve(index, range.start, &offset);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextPositionIndexResolve(index, range.end, &offset);
        if (status == UMI_STATUS_OK)
        {
            LocationEntry *entry = &catalogue->entries[i];
            entry->uri = malloc(uri_bytes + 1U);
            if (entry->uri == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
            else
            {
                memcpy(entry->uri, uri, uri_bytes + 1U);
                entry->value =
                    (UmiLanguageSourceLocation){.uri = entry->uri, .target = range, .selection = range};
            }
        }
    }
    UmiEditorTextPositionIndexDestroy(index);
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageLocationCatalogueDestroy(catalogue);
    return status;
}
