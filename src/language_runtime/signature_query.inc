/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/signature_query.inc
 * PURPOSE: Expose captured parameter-help requests through the shared source-query lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus SignatureInputs(const UmiLanguageSignatureRequest *request,
                                 UmiLanguageSignatureReport *report, UmiLanguageSignatureCatalogue **out,
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
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
static void SignatureReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageSignatureReport *out)
{
    *out = (UmiLanguageSignatureReport){source->started,      source->initialized,  source->document_opened,
                                        source->query_status, source->close_status, source->shutdown_status,
                                        source->choices};
}
UmiStatus UmiLanguageSignatureQueryNative(const UmiLanguageServerProfile *profile,
                                          const char *working_directory,
                                          const UmiLanguageSignatureRequest *request,
                                          const UmiCancellationToken *cancel,
                                          UmiLanguageSignatureReport *out_report,
                                          UmiLanguageSignatureCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = SignatureInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SIGNATURE};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    SignatureReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.signatures;
    return status;
}
UmiStatus UmiLanguageSignatureQueryOnServer(UmiLanguageRuntimeServer *server,
                                            const UmiLanguageSignatureRequest *request,
                                            const UmiCancellationToken *cancel,
                                            UmiLanguageSignatureReport *out_report,
                                            UmiLanguageSignatureCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = SignatureInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_SIGNATURE};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    SignatureReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.signatures;
    return status;
}
