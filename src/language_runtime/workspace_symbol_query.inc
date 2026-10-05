/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/workspace_symbol_query.inc
 * PURPOSE: Expose complete project-symbol searches through the shared captured-source connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Reuse the document-symbol result owner while selecting project scope in the
 * connection policy. Input rejection still occurs before taking server ownership. */
UmiStatus UmiLanguageWorkspaceSymbolQueryNative(const UmiLanguageServerProfile *profile,
                                                const char *working_directory,
                                                const UmiLanguageWorkspaceSymbolRequest *request,
                                                const UmiCancellationToken *cancel,
                                                UmiLanguageSymbolReport *out_report,
                                                UmiLanguageSymbolCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status =
        SymbolInputs(request == NULL ? NULL : &request->source, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (request->query == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SYMBOLS, .workspace_symbol_query = request->query};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    SymbolReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.symbols;
    return status;
}

/* Reuse the document-symbol result owner while selecting project scope in the
 * connection policy. Input rejection still occurs before taking server ownership. */
UmiStatus UmiLanguageWorkspaceSymbolQueryOnServer(UmiLanguageRuntimeServer *server,
                                                  const UmiLanguageWorkspaceSymbolRequest *request,
                                                  const UmiCancellationToken *cancel,
                                                  UmiLanguageSymbolReport *out_report,
                                                  UmiLanguageSymbolCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status =
        SymbolInputs(request == NULL ? NULL : &request->source, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (request->query == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SYMBOLS, .workspace_symbol_query = request->query};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    SymbolReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.symbols;
    return status;
}
