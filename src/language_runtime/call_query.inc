/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/call_query.inc
 * PURPOSE: Expose direct call relationship queries through the shared captured-source transport owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus CallQueryInputs(const UmiLanguageCallRequest *request,
                                 UmiLanguageCompletionQueryReport *report, UmiLanguageCallResult **out)
{
    if (out != NULL)
        *out = NULL;
    if (report != NULL)
    {
        memset(report, 0, sizeof(*report));
        report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request == NULL || report == NULL || out == NULL ||
        (request->direction != UMI_LANGUAGE_CALL_INCOMING &&
         request->direction != UMI_LANGUAGE_CALL_OUTGOING))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCallQueryNative(const UmiLanguageServerProfile *profile, const char *directory,
                                     const UmiLanguageCallRequest *request,
                                     const UmiCancellationToken *cancel,
                                     UmiLanguageCompletionQueryReport *report, UmiLanguageCallResult **out)
{
    UmiStatus status = CallQueryInputs(request, report, out);
    if (status != UMI_STATUS_OK)
        return status;
    SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CALL_HIERARCHY, .call_direction = request->direction};
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, directory, &request->source, cancel, report, &policy, &result);
    if (status == UMI_STATUS_OK)
        *out = result.calls;
    return status;
}
UmiStatus UmiLanguageCallQueryOnServer(UmiLanguageRuntimeServer *server,
                                       const UmiLanguageCallRequest *request,
                                       const UmiCancellationToken *cancel,
                                       UmiLanguageCompletionQueryReport *report, UmiLanguageCallResult **out)
{
    UmiStatus status = CallQueryInputs(request, report, out);
    if (status != UMI_STATUS_OK)
        return status;
    SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CALL_HIERARCHY, .call_direction = request->direction};
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &request->source, cancel, report, &policy, &result);
    if (status == UMI_STATUS_OK)
        *out = result.calls;
    return status;
}
