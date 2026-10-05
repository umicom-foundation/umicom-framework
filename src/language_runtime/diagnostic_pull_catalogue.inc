/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_pull_catalogue.inc
 * PURPOSE: Decode complete explicit diagnostic reports into the shared owned diagnostic catalogue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The response tree owns every diagnostic object unchanged. Reading rows from
 * that tree preserves provider data without rebuilding or truncating JSON. */
UmiStatus UmiLanguageDiagnosticCatalogueReadPullResponse(const void *json, size_t bytes,
                                                         uint64_t expected_request_id,
                                                         const char *document_uri,
                                                         const UmiCancellationToken *cancel,
                                                         UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL || document_uri == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t uri_bytes = strlen(document_uri);
    UmiStatus status = LocationUriValidate(document_uri, uri_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    UmiLanguageDiagnosticCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    int result = -1, kind = -1, items = -1, result_id = -1, related = -1;
    status = UmiLanguageResponseTreeRead(json, bytes, expected_request_id, &limits, cancel, &catalogue->tree,
                                         &result);
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, result, "kind", 1, &kind);
    char name[32];
    if (status == UMI_STATUS_OK)
        status = CompletionText(catalogue->tree, kind, name, sizeof(name));
    /* No previous result was supplied. Accepting 'unchanged' here would lose
     * all diagnostics while falsely presenting the report as complete. */
    if (status == UMI_STATUS_OK && strcmp(name, "unchanged") == 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && strcmp(name, "full") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, result, "items", 1, &items);
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, result, "resultId", 0, &result_id);
    if (status == UMI_STATUS_OK && result_id >= 0)
        status = DiagnosticText(catalogue->tree, result_id, 8192U, &catalogue->pull_result_id);
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, result, "relatedDocuments", 0, &related);
    if (status == UMI_STATUS_OK && related >= 0)
    {
        if (UmiJsonTreeKind(catalogue->tree, related) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            status = UMI_STATUS_PARSE_ERROR;
        else if (UmiJsonTreeCount(catalogue->tree, related) != 0U)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
    {
        catalogue->uri = malloc(uri_bytes + 1U);
        if (catalogue->uri == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            memcpy(catalogue->uri, document_uri, uri_bytes + 1U);
    }
    if (status == UMI_STATUS_OK)
        status = DiagnosticReadRows(catalogue, items, cancel);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageDiagnosticCataloguePullResultId(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                     const char **out)
{
    if (out != NULL)
        *out = NULL;
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->pull_result_id == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->pull_result_id;
    return UMI_STATUS_OK;
}
