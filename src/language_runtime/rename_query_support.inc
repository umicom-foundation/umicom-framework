/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/rename_query_support.inc
 * PURPOSE: Validate rename preparation and captured-document changes before publishing a proposal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Keep wire preparation checks beside the transport owner so every application
 * receives the same range, encoding and version guarantees. No URI is opened. */
static UmiStatus QueryRenameName(const char *name)
{
    if (name == NULL || name[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(name);
    if (bytes > 1024U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < bytes; ++i)
        if ((unsigned char)name[i] < 0x20U || (unsigned char)name[i] == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    return QueryText(name, bytes);
}
static UmiStatus QueryRenamePoint(const UmiJsonTree *tree, int range, const char *name,
                                  const UmiEditorTextBufferView *source, size_t *out)
{
    int point = -1, line = -1, column = -1;
    uint32_t row = 0U, col = 0U;
    UmiStatus status = CompletionMember(tree, range, name, &point);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, point, "line", &line);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, point, "character", &column);
    if (status == UMI_STATUS_OK)
        status = CompletionInteger(tree, line, &row);
    if (status == UMI_STATUS_OK)
        status = CompletionInteger(tree, column, &col);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewResolvePosition(source, (UmiEditorTextPosition){row, col}, out);
    return status;
}
static UmiStatus QueryRenamePreparation(const char *json, uint64_t id,
                                        const UmiLanguageCompletionRequest *request,
                                        const UmiCancellationToken *cancel)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 32U};
    int result = -1, range = -1, placeholder = -1, default_behavior = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(json, strlen(json), id, &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK && UmiJsonTreeIsNull(tree, result))
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "defaultBehavior", &default_behavior);
    /* No default identifier detector is advertised. A server must supply an
     * actual range; guessing from punctuation would change language semantics. */
    if (status == UMI_STATUS_OK && default_behavior >= 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "range", &range);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "placeholder", &placeholder);
    if (status == UMI_STATUS_OK && range >= 0)
    {
        char *text = malloc(4097U);
        if (text == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            status = CompletionText(tree, placeholder, text, 4097U);
            free(text);
        }
    }
    else if (status == UMI_STATUS_OK)
    {
        if (placeholder >= 0)
            status = UMI_STATUS_PARSE_ERROR;
        range = result;
    }
    UmiEditorTextBufferView source = {0};
    source.struct_size = (uint32_t)sizeof(source);
    source.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    source.bytes = request->source;
    source.byte_count = request->source_bytes;
    source.capacity = request->source_bytes;
    size_t first = 0U, last = 0U;
    if (status == UMI_STATUS_OK)
        status = QueryRenamePoint(tree, range, "start", &source, &first);
    if (status == UMI_STATUS_OK)
        status = QueryRenamePoint(tree, range, "end", &source, &last);
    if (status == UMI_STATUS_OK &&
        (first > last || request->cursor_offset < first || request->cursor_offset > last))
        status = UMI_STATUS_PARSE_ERROR;
    UmiJsonTreeDestroy(tree);
    return status;
}
static UmiStatus QueryRenameResult(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageWorkspaceEditCatalogue **out)
{
    *out = NULL;
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    UmiStatus status =
        UmiLanguageWorkspaceEditCatalogueReadResponse(json, strlen(json), id, cancel, &catalogue);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageWorkspaceEditCatalogueCount(catalogue); ++i)
    {
        UmiLanguageWorkspaceDocumentChange document;
        status = UmiLanguageWorkspaceEditCatalogueDocument(catalogue, i, &document);
        if (status == UMI_STATUS_OK && strcmp(document.uri, request->document_uri) == 0)
        {
            /* Only this exact URI was sent as version 1. Never borrow that
             * identity or draft to validate a different document's edits. */
            const int32_t version = 1;
            UmiLanguageTextEditPreview *preview = NULL;
            status =
                UmiLanguageWorkspaceEditCataloguePreview(catalogue, i, request->source, request->source_bytes,
                                                         request->cursor_offset, &version, cancel, &preview);
            UmiLanguageTextEditPreviewDestroy(preview);
        }
    }
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    return status;
}
