/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/rename_query.inc
 * PURPOSE: Expose captured rename requests through the shared source-query lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus RenameInputs(const UmiLanguageRenameRequest *request, UmiLanguageRenameReport *report,
                              UmiLanguageWorkspaceEditCatalogue **out, UmiLanguageCompletionRequest *source)
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
static void RenameReport(const UmiLanguageCompletionQueryReport *source, UmiLanguageRenameReport *out)
{
    *out = (UmiLanguageRenameReport){source->started,      source->initialized,  source->document_opened,
                                     source->query_status, source->close_status, source->shutdown_status,
                                     source->choices};
}
UmiStatus UmiLanguageRenameQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                       const UmiLanguageRenameRequest *request,
                                       const UmiCancellationToken *cancel,
                                       UmiLanguageRenameReport *out_report,
                                       UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = RenameInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_RENAME, .rename_name = request->new_name};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    RenameReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.workspace_edits;
    return status;
}
UmiStatus UmiLanguageRenameQueryOnServer(UmiLanguageRuntimeServer *server,
                                         const UmiLanguageRenameRequest *request,
                                         const UmiCancellationToken *cancel,
                                         UmiLanguageRenameReport *out_report,
                                         UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = RenameInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_RENAME, .rename_name = request->new_name};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    RenameReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.workspace_edits;
    return status;
}

/* Additional drafts share the existing temporary connection and cleanup owner. */
UmiStatus UmiLanguageRenameQueryNativeWithSources(const UmiLanguageServerProfile *profile,
                                                  const char *working_directory,
                                                  const UmiLanguageRenameRequest *request,
                                                  const UmiLanguageQuerySource *additional_sources,
                                                  size_t source_count, const UmiCancellationToken *cancel,
                                                  UmiLanguageRenameReport *out_report,
                                                  UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = RenameInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_RENAME,
                                      .rename_name = request->new_name,
                                      .additional_sources = additional_sources,
                                      .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, &source, cancel, &report, &policy, &result);
    RenameReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.workspace_edits;
    return status;
}

/* Additional drafts share the existing temporary connection and cleanup owner. */
UmiStatus UmiLanguageRenameQueryOnServerWithSources(UmiLanguageRuntimeServer *server,
                                                    const UmiLanguageRenameRequest *request,
                                                    const UmiLanguageQuerySource *additional_sources,
                                                    size_t source_count, const UmiCancellationToken *cancel,
                                                    UmiLanguageRenameReport *out_report,
                                                    UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    UmiLanguageCompletionRequest source;
    UmiStatus status = RenameInputs(request, out_report, out_catalogue, &source);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {.kind = SOURCE_QUERY_RENAME,
                                      .rename_name = request->new_name,
                                      .additional_sources = additional_sources,
                                      .source_count = source_count};
    UmiLanguageCompletionQueryReport report;
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, &source, cancel, &report, &policy, &result);
    RenameReport(&report, out_report);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.workspace_edits;
    return status;
}
