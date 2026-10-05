/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/hover_query.inc
 * PURPOSE: Expose read-only source lookup through the shared temporary protocol owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus HoverInputs(const UmiLanguageHoverRequest *request, UmiLanguageHoverReport *report,
                             UmiLanguageHoverDocument **out, UmiLanguageCompletionRequest *source)
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
static void HoverReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageHoverReport *out)
{
    *out = (UmiLanguageHoverReport){source->started,      source->initialized,  source->document_opened,
                                    source->query_status, source->close_status, source->shutdown_status,
                                    source->choices};
}
UmiStatus UmiLanguageHoverQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                      const UmiLanguageHoverRequest *request,
                                      const UmiCancellationToken *cancel, UmiLanguageHoverReport *out_report,
                                      UmiLanguageHoverDocument **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = HoverInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    /* Named operations prevent combinations of incompatible source intents.
     * Retain the positional declaration for review. */
#if 0
    const SourceQueryPolicy policy = {0, 0U, 0, 1};
#endif
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_HOVER};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    HoverReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.hover;
    return status;
}
UmiStatus UmiLanguageHoverQueryOnServer(UmiLanguageRuntimeServer *server,
                                        const UmiLanguageHoverRequest *request,
                                        const UmiCancellationToken *cancel,
                                        UmiLanguageHoverReport *out_report,
                                        UmiLanguageHoverDocument **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = HoverInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    /* Named operations prevent combinations of incompatible source intents.
     * Retain the positional declaration for review. */
#if 0
    const SourceQueryPolicy policy = {0, 0U, 0, 1};
#endif
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_HOVER};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    HoverReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.hover;
    return status;
}
