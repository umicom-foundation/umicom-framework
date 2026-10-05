/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_query.inc
 * PURPOSE: Expose captured diagnostic requests through the shared source-query lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus DiagnosticInputs(const UmiLanguageDiagnosticRequest *request,
                                  UmiLanguageDiagnosticReport *report, UmiLanguageDiagnosticCatalogue **out,
                                  UmiLanguageCompletionRequest *source)
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
static void DiagnosticReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageDiagnosticReport *out)
{
    *out = (UmiLanguageDiagnosticReport){source->started,      source->initialized,  source->document_opened,
                                         source->query_status, source->close_status, source->shutdown_status,
                                         source->choices};
}
UmiStatus UmiLanguageDiagnosticQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageDiagnosticRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageDiagnosticReport *out_report,
                                           UmiLanguageDiagnosticCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = DiagnosticInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_DIAGNOSTICS};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    DiagnosticReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.diagnostics;
    return status;
}
UmiStatus UmiLanguageDiagnosticQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageDiagnosticRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageDiagnosticReport *out_report,
                                             UmiLanguageDiagnosticCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = DiagnosticInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_DIAGNOSTICS};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    DiagnosticReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.diagnostics;
    return status;
}

/* Select the explicit request policy while retaining shared source and child ownership. */
UmiStatus UmiLanguageDiagnosticQueryPullNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageDiagnosticRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageDiagnosticReport *out_report,
                                               UmiLanguageDiagnosticCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = DiagnosticInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_DIAGNOSTICS, .pull_diagnostics = 1};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    DiagnosticReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.diagnostics;
    return status;
}

/* Select the explicit request policy while retaining shared source and child ownership. */
UmiStatus UmiLanguageDiagnosticQueryPullOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageDiagnosticRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageDiagnosticReport *out_report,
                                                 UmiLanguageDiagnosticCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = DiagnosticInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_DIAGNOSTICS, .pull_diagnostics = 1};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    DiagnosticReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.diagnostics;
    return status;
}
