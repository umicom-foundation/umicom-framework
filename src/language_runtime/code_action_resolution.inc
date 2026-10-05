/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/code_action_resolution.inc
 * PURPOSE: Publish complete resolved text-action catalogues only when immutable provider metadata is preserved.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Resolution is allowed to supply only the advertised edit property. Compare
 * complete immutable values rather than rebuilding a title/data-only action,
 * which could silently lose diagnostics or an extension's required metadata. */
static UmiStatus ActionResolutionIdentity(const UmiJsonTree *original, int item, const UmiJsonTree *resolved,
                                          int result, const UmiCancellationToken *cancel)
{
    if (UmiJsonTreeKind(resolved, result) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    if (UmiJsonTreeCount(resolved, result) != UmiJsonTreeCount(original, item) + 1U)
        return UMI_STATUS_INVALID_STATE;
    if (UmiJsonTreeCount(resolved, result) > 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *name = malloc(4097U);
    if (name == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    int edit_seen = 0;
    for (int key = UmiJsonTreeFirst(resolved, result); status == UMI_STATUS_OK && key >= 0;)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        int value = UmiJsonTreeNext(resolved, key), unique = -1;
        status = UmiJsonTreeText(resolved, key, name, 4097U);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeMember(resolved, result, name, &unique);
        if (status == UMI_STATUS_OK && strcmp(name, "edit") == 0)
        {
            if (UmiJsonTreeKind(resolved, value) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
                status = UMI_STATUS_PARSE_ERROR;
            else
                edit_seen = 1;
        }
        else if (status == UMI_STATUS_OK)
        {
            int before = -1, equal = 0;
            status = UmiJsonTreeMember(original, item, name, &before);
            if (status == UMI_STATUS_NOT_FOUND)
                status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeValuesEqual(original, before, resolved, value, cancel, &equal);
            if (status == UMI_STATUS_OK && !equal)
                status = UMI_STATUS_INVALID_STATE;
        }
        key = UmiJsonTreeNext(resolved, value);
    }
    free(name);
    if (status == UMI_STATUS_OK && !edit_seen)
        status = UMI_STATUS_INVALID_STATE;
    return status;
}
UmiStatus UmiLanguageCodeActionCatalogueWithResolvedResponse(const UmiLanguageCodeActionCatalogue *catalogue,
                                                             size_t index, const void *json, size_t bytes,
                                                             uint64_t expected_request_id,
                                                             const UmiCancellationToken *cancel,
                                                             UmiLanguageCodeActionCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (catalogue == NULL || json == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    const CodeActionRow *row = &catalogue->rows[index];
    if (row->disabled)
        return UMI_STATUS_PERMISSION_DENIED;
    if (row->has_command)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (row->edit >= 0)
        return UMI_STATUS_INVALID_STATE;
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, bytes, expected_request_id, &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK)
        status = ActionResolutionIdentity(catalogue->tree, row->item, tree, result, cancel);
    const char *replacement = NULL;
    size_t replacement_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &replacement, &replacement_bytes);
    size_t total = 2U;
    /* Measure the entire publication first. Repeated resolve calls cannot grow
     * a catalogue past its shared budget or expose only the last edited row. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const char *span = replacement;
        size_t length = replacement_bytes;
        if (i != index)
            status = UmiLanguageCodeActionCatalogueItemJson(catalogue, i, &span, &length);
        size_t comma = i != 0U ? 1U : 0U;
        if (status == UMI_STATUS_OK &&
            (comma > 1024U * 1024U - total || length > 1024U * 1024U - total - comma))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
            total += length + comma;
    }
    char *complete = status == UMI_STATUS_OK ? malloc(total + 1U) : NULL;
    if (status == UMI_STATUS_OK && complete == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 1U;
    if (complete != NULL)
        complete[0] = '[';
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const char *span = replacement;
        size_t length = replacement_bytes;
        if (i != index)
            status = UmiLanguageCodeActionCatalogueItemJson(catalogue, i, &span, &length);
        if (status == UMI_STATUS_OK)
        {
            if (i != 0U)
                complete[used++] = ',';
            memcpy(complete + used, span, length);
            used += length;
        }
    }
    if (status == UMI_STATUS_OK)
    {
        complete[used++] = ']';
        complete[used] = '\0';
        status = UmiLanguageCodeActionCatalogueCreate(complete, used, cancel, out);
    }
    free(complete);
    UmiJsonTreeDestroy(tree);
    return status;
}
