/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/symbol_query.inc
 * PURPOSE: Expose captured document-symbol requests through the shared source-query lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus SymbolInputs(const UmiLanguageSymbolRequest *request, UmiLanguageSymbolReport *report,
                              UmiLanguageSymbolCatalogue **out, UmiLanguageCompletionRequest *source)
{
    if (out != NULL)
        *out = NULL;
    if (report != NULL)
    {
        memset(report, 0, sizeof(*report));
        report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request == NULL || report == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, 0U,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
static void SymbolReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageSymbolReport *out)
{
    *out = (UmiLanguageSymbolReport){source->started,      source->initialized,  source->document_opened,
                                     source->query_status, source->close_status, source->shutdown_status,
                                     source->choices};
}
UmiStatus UmiLanguageSymbolQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                       const UmiLanguageSymbolRequest *request,
                                       const UmiCancellationToken *cancel,
                                       UmiLanguageSymbolReport *out_report,
                                       UmiLanguageSymbolCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = SymbolInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SYMBOLS};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    SymbolReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.symbols;
    return status;
}
UmiStatus UmiLanguageSymbolQueryOnServer(UmiLanguageRuntimeServer *server,
                                         const UmiLanguageSymbolRequest *request,
                                         const UmiCancellationToken *cancel,
                                         UmiLanguageSymbolReport *out_report,
                                         UmiLanguageSymbolCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = SymbolInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SYMBOLS};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    SymbolReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.symbols;
    return status;
}
