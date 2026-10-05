/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/code_action_query.inc
 * PURPOSE: Expose captured code-action requests through the shared source-query lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus CodeActionInputs(const UmiLanguageCodeActionRequest *request,
                                  UmiLanguageCodeActionReport *report, UmiLanguageCodeActionCatalogue **out,
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
                                             request->source,    request->source_bytes, request->range_start,
                                             request->timeout_ms};
    return UMI_STATUS_OK;
}
static void CodeActionReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageCodeActionReport *out)
{
    *out = (UmiLanguageCodeActionReport){source->started,      source->initialized,  source->document_opened,
                                         source->query_status, source->close_status, source->shutdown_status,
                                         source->choices};
}
UmiStatus UmiLanguageCodeActionQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageCodeActionRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageCodeActionReport *out_report,
                                           UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CODE_ACTION, .range_end = request->range_end};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}
UmiStatus UmiLanguageCodeActionQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageCodeActionRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageCodeActionReport *out_report,
                                             UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CODE_ACTION, .range_end = request->range_end};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Fresh diagnostics and the action request share this one source owner. */
UmiStatus UmiLanguageCodeActionQueryNativeWithDiagnostics(const UmiLanguageServerProfile *profile,
                                                          const char *working_directory,
                                                          const UmiLanguageCodeActionRequest *request,
                                                          const UmiCancellationToken *cancel,
                                                          UmiLanguageCodeActionReport *out_report,
                                                          UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION, .range_end = request->range_end, .diagnostic_context = 1};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Fresh diagnostics and the action request share this one source owner. */
UmiStatus UmiLanguageCodeActionQueryOnServerWithDiagnostics(UmiLanguageRuntimeServer *server,
                                                            const UmiLanguageCodeActionRequest *request,
                                                            const UmiCancellationToken *cancel,
                                                            UmiLanguageCodeActionReport *out_report,
                                                            UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION, .range_end = request->range_end, .diagnostic_context = 1};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Additional source synchronization uses the shared connection lifecycle;
 * diagnostics remain an explicit primary-source context in that connection. */
UmiStatus UmiLanguageCodeActionQueryNativeWithSources(
    const UmiLanguageServerProfile *profile, const char *working_directory,
    const UmiLanguageCodeActionRequest *request, const UmiLanguageQuerySource *additional_sources,
    size_t source_count, int include_diagnostics, const UmiCancellationToken *cancel,
    UmiLanguageCodeActionReport *out_report, UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (include_diagnostics != 0 && include_diagnostics != 1)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CODE_ACTION,
                                      .range_end = request->range_end,
                                      .diagnostic_context = include_diagnostics,
                                      .additional_sources = additional_sources,
                                      .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Additional source synchronization uses the shared connection lifecycle;
 * diagnostics remain an explicit primary-source context in that connection. */
UmiStatus UmiLanguageCodeActionQueryOnServerWithSources(UmiLanguageRuntimeServer *server,
                                                        const UmiLanguageCodeActionRequest *request,
                                                        const UmiLanguageQuerySource *additional_sources,
                                                        size_t source_count, int include_diagnostics,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageCodeActionReport *out_report,
                                                        UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (include_diagnostics != 0 && include_diagnostics != 1)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_CODE_ACTION,
                                      .range_end = request->range_end,
                                      .diagnostic_context = include_diagnostics,
                                      .additional_sources = additional_sources,
                                      .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Select diagnostic delivery without changing the source owner or accepting cached provider data. */
UmiStatus UmiLanguageCodeActionQueryNativeWithContext(
    const UmiLanguageServerProfile *profile, const char *working_directory,
    const UmiLanguageCodeActionRequest *request, const UmiLanguageQuerySource *additional_sources,
    size_t source_count, UmiLanguageActionDiagnosticMode diagnostic_mode, const UmiCancellationToken *cancel,
    UmiLanguageCodeActionReport *out_report, UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .additional_sources = additional_sources,
        .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Select diagnostic delivery without changing the source owner or accepting cached provider data. */
UmiStatus UmiLanguageCodeActionQueryOnServerWithContext(
    UmiLanguageRuntimeServer *server, const UmiLanguageCodeActionRequest *request,
    const UmiLanguageQuerySource *additional_sources, size_t source_count,
    UmiLanguageActionDiagnosticMode diagnostic_mode, const UmiCancellationToken *cancel,
    UmiLanguageCodeActionReport *out_report, UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .additional_sources = additional_sources,
        .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Compose filtering with the same source and diagnostic ownership used by
 * unfiltered queries. Invalid options fail before the server is touched. */
UmiStatus UmiLanguageCodeActionQueryNativeWithOptions(const UmiLanguageServerProfile *profile,
                                                      const char *working_directory,
                                                      const UmiLanguageCodeActionRequest *request,
                                                      const UmiLanguageCodeActionQueryOptions *options,
                                                      const UmiCancellationToken *cancel,
                                                      UmiLanguageCodeActionReport *out_report,
                                                      UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (options == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *kind = NULL;
    status = UmiLanguageCodeActionFilterName(options->filter, &kind);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageActionDiagnosticMode diagnostic_mode = options->diagnostics;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .action_filter = options->filter,
        .action_only_kind = kind,
        .additional_sources = options->additional_sources,
        .source_count = options->source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Compose filtering with the same source and diagnostic ownership used by
 * unfiltered queries. Invalid options fail before the server is touched. */
UmiStatus UmiLanguageCodeActionQueryOnServerWithOptions(UmiLanguageRuntimeServer *server,
                                                        const UmiLanguageCodeActionRequest *request,
                                                        const UmiLanguageCodeActionQueryOptions *options,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageCodeActionReport *out_report,
                                                        UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (options == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *kind = NULL;
    status = UmiLanguageCodeActionFilterName(options->filter, &kind);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageActionDiagnosticMode diagnostic_mode = options->diagnostics;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .action_filter = options->filter,
        .action_only_kind = kind,
        .additional_sources = options->additional_sources,
        .source_count = options->source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Keep resolution an explicit query policy; compatibility entry points do
 * not advertise or request deferred edits. Invalid limits fail before launch. */
UmiStatus UmiLanguageCodeActionQueryNativeWithResolution(
    const UmiLanguageServerProfile *profile, const char *working_directory,
    const UmiLanguageCodeActionRequest *request, const UmiLanguageCodeActionQueryOptions *options,
    size_t maximum_actions, const UmiCancellationToken *cancel, UmiLanguageCodeActionReport *out_report,
    UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (options == NULL || maximum_actions == 0U || maximum_actions > 32U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *kind = NULL;
    status = UmiLanguageCodeActionFilterName(options->filter, &kind);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageActionDiagnosticMode diagnostic_mode = options->diagnostics;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .resolve_action_limit = maximum_actions,
        .action_filter = options->filter,
        .action_only_kind = kind,
        .additional_sources = options->additional_sources,
        .source_count = options->source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}

/* Keep resolution an explicit query policy; compatibility entry points do
 * not advertise or request deferred edits. Invalid limits fail before launch. */
UmiStatus UmiLanguageCodeActionQueryOnServerWithResolution(UmiLanguageRuntimeServer *server,
                                                           const UmiLanguageCodeActionRequest *request,
                                                           const UmiLanguageCodeActionQueryOptions *options,
                                                           size_t maximum_actions,
                                                           const UmiCancellationToken *cancel,
                                                           UmiLanguageCodeActionReport *out_report,
                                                           UmiLanguageCodeActionCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = CodeActionInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    if (options == NULL || maximum_actions == 0U || maximum_actions > 32U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *kind = NULL;
    status = UmiLanguageCodeActionFilterName(options->filter, &kind);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageActionDiagnosticMode diagnostic_mode = options->diagnostics;
    if (diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION &&
        diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const SourceQueryPolicy policy = {
        .kind = SOURCE_QUERY_CODE_ACTION,
        .range_end = request->range_end,
        .diagnostic_context = diagnostic_mode != UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        .pull_diagnostics = diagnostic_mode == UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL,
        .resolve_action_limit = maximum_actions,
        .action_filter = options->filter,
        .action_only_kind = kind,
        .additional_sources = options->additional_sources,
        .source_count = options->source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    CodeActionReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.actions;
    return status;
}
