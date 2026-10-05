/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/selection_locations.inc
 * PURPOSE: Decode one complete enclosing selection chain without recursion or partial publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiLanguageLocationCatalogueReadSelectionResponse(const void *json, size_t bytes,
                                                            uint64_t expected_id, const char *uri,
                                                            UmiEditorTextPosition position,
                                                            const UmiCancellationToken *cancel,
                                                            UmiLanguageLocationCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (uri == NULL || position.line > INT32_MAX || position.utf16_column > INT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t uri_bytes = strlen(uri);
    UmiStatus status = LocationUriValidate(uri, uri_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    UmiJsonTree *tree = NULL;
    int result = -1;
    /* The tree parser bounds container depth independently of the 64 semantic
     * ranges. Unknown extension data remains subject to the JSON budget. */
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 80U};
    status = UmiLanguageResponseTreeRead(json, bytes, expected_id, &limits, cancel, &tree, &result);
    int node = -1;
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(tree, result))
    {
        if (UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
            UmiJsonTreeCount(tree, result) != 1U)
            status = UMI_STATUS_PARSE_ERROR;
        else
            node = UmiJsonTreeFirst(tree, result);
    }
    UmiLanguageSourceRange ranges[64];
    size_t count = 0U;
    UmiLanguageSourceRange contained = {position, position};
    /* Walk parents iteratively. Validate the complete chain before allocating
     * an exposed catalogue; a bad outer range must not leave usable inner rows. */
    while (status == UMI_STATUS_OK && node >= 0)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (count == 64U)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        int range = -1, parent = -1;
        status = LocationMember(tree, node, "range", 1, &range);
        if (status == UMI_STATUS_OK)
            status = LocationRange(tree, range, &ranges[count]);
        if (status == UMI_STATUS_OK && (LocationCompare(ranges[count].start, contained.start) > 0 ||
                                        LocationCompare(ranges[count].end, contained.end) < 0))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, node, "parent", 0, &parent);
        if (status == UMI_STATUS_OK)
        {
            contained = ranges[count++];
            node = parent;
        }
    }
    UmiJsonTreeDestroy(tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageLocationCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->count = count;
    if (count != 0U)
    {
        catalogue->entries = calloc(count, sizeof(*catalogue->entries));
        if (catalogue->entries == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        LocationEntry *entry = &catalogue->entries[i];
        entry->uri = malloc(uri_bytes + 1U);
        if (entry->uri == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        memcpy(entry->uri, uri, uri_bytes + 1U);
        entry->value =
            (UmiLanguageSourceLocation){.uri = entry->uri, .target = ranges[i], .selection = ranges[i]};
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageLocationCatalogueDestroy(catalogue);
    return status;
}
