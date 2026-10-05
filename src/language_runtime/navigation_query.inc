/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/navigation_query.inc
 * PURPOSE: Expose definition and reference lookup through the shared temporary protocol owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Extend the existing strict navigation contract to type and implementation lookup without accepting unknown operation values.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus NavigationInputs(const UmiLanguageNavigationRequest *request,
                                  UmiLanguageNavigationReport *report, UmiLanguageLocationCatalogue **out,
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
    if ((request->kind != UMI_LANGUAGE_NAVIGATION_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_REFERENCES) ||
        (request->include_declaration != 0 && request->include_declaration != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
#endif
/* Accept syntax-selection requests as an explicit read-only navigation intent while retaining strict rejection of unknown values.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus NavigationInputs(const UmiLanguageNavigationRequest *request,
                                  UmiLanguageNavigationReport *report, UmiLanguageLocationCatalogue **out,
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
    if ((request->kind != UMI_LANGUAGE_NAVIGATION_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_REFERENCES &&
         request->kind != UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION) ||
        (request->include_declaration != 0 && request->include_declaration != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
#endif
/* Validate folding discovery as a distinct read-only operation without weakening the existing navigation contract.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus NavigationInputs(const UmiLanguageNavigationRequest *request,
                                  UmiLanguageNavigationReport *report, UmiLanguageLocationCatalogue **out,
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
    if ((request->kind != UMI_LANGUAGE_NAVIGATION_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_REFERENCES &&
         request->kind != UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES) ||
        (request->include_declaration != 0 && request->include_declaration != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
#endif
static UmiStatus NavigationInputs(const UmiLanguageNavigationRequest *request,
                                  UmiLanguageNavigationReport *report, UmiLanguageLocationCatalogue **out,
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
    if ((request->kind != UMI_LANGUAGE_NAVIGATION_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_REFERENCES &&
         request->kind != UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION &&
         request->kind != UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES &&
         request->kind != UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES) ||
        (request->include_declaration != 0 && request->include_declaration != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    *source = (UmiLanguageCompletionRequest){request->root_uri,  request->document_uri, request->language_id,
                                             request->source,    request->source_bytes, request->caret,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
static void NavigationReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageNavigationReport *out)
{
    *out = (UmiLanguageNavigationReport){source->started,      source->initialized,  source->document_opened,
                                         source->query_status, source->close_status, source->shutdown_status,
                                         source->choices};
}
/* Select an explicit navigation capability while reusing captured source, owned location results and connection cleanup.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageNavigationRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageNavigationReport *out_report,
                                           UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION
                                                  ? SOURCE_QUERY_DEFINITION
                                                  : SOURCE_QUERY_REFERENCES,
                                      .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
/* Use the shared captured-source connection and cleanup owner for enclosing selection ranges.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageNavigationRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageNavigationReport *out_report,
                                           UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION        ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION  ? SOURCE_QUERY_IMPLEMENTATION
                                                                           : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
/* Reuse captured-source ownership and connection shutdown for complete-line folding discovery.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageNavigationRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageNavigationReport *out_report,
                                           UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION         ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION  ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION   ? SOURCE_QUERY_IMPLEMENTATION
                : request->kind == UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES ? SOURCE_QUERY_SELECTION_RANGES
                                                                            : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
UmiStatus UmiLanguageNavigationQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageNavigationRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageNavigationReport *out_report,
                                           UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION         ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION  ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION   ? SOURCE_QUERY_IMPLEMENTATION
                : request->kind == UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES ? SOURCE_QUERY_SELECTION_RANGES
                                                                            : request->kind == UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES ? SOURCE_QUERY_FOLDING_RANGES
 : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
/* Select an explicit navigation capability while reusing captured source, owned location results and connection cleanup.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageNavigationRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageNavigationReport *out_report,
                                             UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION
                                                  ? SOURCE_QUERY_DEFINITION
                                                  : SOURCE_QUERY_REFERENCES,
                                      .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
/* Use the shared captured-source connection and cleanup owner for enclosing selection ranges.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageNavigationRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageNavigationReport *out_report,
                                             UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION        ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION  ? SOURCE_QUERY_IMPLEMENTATION
                                                                           : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
/* Reuse captured-source ownership and connection shutdown for complete-line folding discovery.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageNavigationQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageNavigationRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageNavigationReport *out_report,
                                             UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION         ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION  ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION   ? SOURCE_QUERY_IMPLEMENTATION
                : request->kind == UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES ? SOURCE_QUERY_SELECTION_RANGES
                                                                            : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
#endif
UmiStatus UmiLanguageNavigationQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageNavigationRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageNavigationReport *out_report,
                                             UmiLanguageLocationCatalogue **out_document)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = NavigationInputs(request, out_report, out_document, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = request->kind == UMI_LANGUAGE_NAVIGATION_DEFINITION         ? SOURCE_QUERY_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION  ? SOURCE_QUERY_TYPE_DEFINITION
                : request->kind == UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION   ? SOURCE_QUERY_IMPLEMENTATION
                : request->kind == UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES ? SOURCE_QUERY_SELECTION_RANGES
                                                                            : request->kind == UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES ? SOURCE_QUERY_FOLDING_RANGES
 : SOURCE_QUERY_REFERENCES,
        .include_declaration = request->include_declaration};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    NavigationReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_document = result.locations;
    return status;
}
