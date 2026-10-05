/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/formatting_query.inc
 * PURPOSE: Expose formatting policy through the shared temporary source-query owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Formatting names its operation explicitly instead of relying on positional mode flags.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus FormattingInputs(const UmiLanguageFormattingRequest *request,
                                  UmiLanguageFormattingReport *report, UmiLanguageTextEditPreview **out,
                                  UmiLanguageCompletionRequest *source, SourceQueryPolicy *policy)
{
    if (out != NULL)
        *out = NULL;
    if (report != NULL)
    {
        memset(report, 0, sizeof(*report));
        report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out == NULL || report == NULL || request == NULL || request->tab_size < 1U ||
        request->tab_size > 32U || (request->insert_spaces != 0 && request->insert_spaces != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    *policy = (SourceQueryPolicy){1, request->tab_size, request->insert_spaces, 0};
    return UMI_STATUS_OK;
}
#endif
static UmiStatus FormattingInputs(const UmiLanguageFormattingRequest *request,
                                  UmiLanguageFormattingReport *report, UmiLanguageTextEditPreview **out,
                                  UmiLanguageCompletionRequest *source, SourceQueryPolicy *policy)
{
    if (out != NULL)
        *out = NULL;
    if (report != NULL)
    {
        memset(report, 0, sizeof(*report));
        report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out == NULL || report == NULL || request == NULL || request->tab_size < 1U ||
        request->tab_size > 32U || (request->insert_spaces != 0 && request->insert_spaces != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    *policy = (SourceQueryPolicy){.kind = SOURCE_QUERY_FORMATTING,
                                  .tab_size = request->tab_size,
                                  .insert_spaces = request->insert_spaces};
    return UMI_STATUS_OK;
}
static void FormattingReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageFormattingReport *out)
{
    *out = (UmiLanguageFormattingReport){source->started,      source->initialized,  source->document_opened,
                                         source->query_status, source->close_status, source->shutdown_status,
                                         source->choices};
}
UmiStatus UmiLanguageFormattingQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageFormattingRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageFormattingReport *out_report,
                                           UmiLanguageTextEditPreview **out_preview)
{
    UmiLanguageCompletionRequest source;
    SourceQueryPolicy policy;
    UmiStatus status = FormattingInputs(request, out_report, out_preview, &source, &policy);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    FormattingReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_preview = result.formatting;
    return status;
}
UmiStatus UmiLanguageFormattingQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageFormattingRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageFormattingReport *out_report,
                                             UmiLanguageTextEditPreview **out_preview)
{
    UmiLanguageCompletionRequest source;
    SourceQueryPolicy policy;
    UmiStatus status = FormattingInputs(request, out_report, out_preview, &source, &policy);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    FormattingReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_preview = result.formatting;
    return status;
}
