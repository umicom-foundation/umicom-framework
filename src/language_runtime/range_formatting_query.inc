/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/range_formatting_query.inc
 * PURPOSE: Expose exact selected-range formatting through the shared source-query and full-preview owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Keep the preview caret independent of the formatting range. Both range
 * boundaries are checked by shared preflight before a process is owned. */
UmiStatus UmiLanguageRangeFormattingQueryNative(const UmiLanguageServerProfile *profile,
                                                const char *working_directory,
                                                const UmiLanguageRangeFormattingRequest *request,
                                                const UmiCancellationToken *cancel,
                                                UmiLanguageFormattingReport *out_report,
                                                UmiLanguageTextEditPreview **out_preview)
{
    UmiLanguageCompletionRequest source;
    SourceQueryPolicy policy;
    UmiStatus status = FormattingInputs(request == NULL ? NULL : &request->source, out_report, out_preview,
                                        &source, &policy);
    if (status != UMI_STATUS_OK)
        return status;
    if (request->start > request->end || request->end > request->source.source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    policy.range_formatting = 1;
    policy.range_start = request->start;
    policy.range_end = request->end;
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    FormattingReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_preview = result.formatting;
    return status;
}

/* Keep the preview caret independent of the formatting range. Both range
 * boundaries are checked by shared preflight before a process is owned. */
UmiStatus UmiLanguageRangeFormattingQueryOnServer(UmiLanguageRuntimeServer *server,
                                                  const UmiLanguageRangeFormattingRequest *request,
                                                  const UmiCancellationToken *cancel,
                                                  UmiLanguageFormattingReport *out_report,
                                                  UmiLanguageTextEditPreview **out_preview)
{
    UmiLanguageCompletionRequest source;
    SourceQueryPolicy policy;
    UmiStatus status = FormattingInputs(request == NULL ? NULL : &request->source, out_report, out_preview,
                                        &source, &policy);
    if (status != UMI_STATUS_OK)
        return status;
    if (request->start > request->end || request->end > request->source.source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    policy.range_formatting = 1;
    policy.range_start = request->start;
    policy.range_end = request->end;
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    FormattingReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_preview = result.formatting;
    return status;
}
