/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/action_diagnostics_query.inc
 * PURPOSE: Build action context from diagnostics published by the same captured-source session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Opaque diagnostic data can identify server-owned fixes. Capture and use it
 * before closing this connection; never accept another session's saved data. */
/* Build action context from either a fresh full pull report or a matching publication in the same connection, preserving provider-owned diagnostic data.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryActionDiagnostics(UmiLanguageRuntimeServer *server,
                                        const UmiLanguageCompletionRequest *request,
                                        const SourceQueryPolicy *policy, UmiClock *clock, uint64_t started,
                                        const UmiCancellationToken *cancel, UmiLanguageRuntimeEnvelope *reply,
                                        char **out_parameters)
{
    *out_parameters = NULL;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiLanguageDiagnosticContext *context = NULL;
    UmiStatus status = QueryAwaitDiagnostics(server, request, clock, started, cancel, reply, &catalogue);
    const int32_t version = 1;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextCreate(
            catalogue, request->document_uri, &version, request->source, request->source_bytes,
            request->cursor_offset, policy->range_end, cancel, &context);
    const char *diagnostics = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextJson(context, &diagnostics, &bytes);
    (void)bytes;
    char *parameters = NULL;
    if (status == UMI_STATUS_OK)
    {
        parameters = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        if (parameters == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = request->source_bytes;
        view.capacity = request->source_bytes;
        UmiEditorTextPosition begin, end;
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &begin);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &end);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageRuntimeJsonWriter writer;
            umi_language_runtime_json_writer_init(&writer, parameters, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
            umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
            umi_language_runtime_json_writer_string(&writer, request->document_uri);
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":");
            umi_language_runtime_json_writer_raw(&writer, diagnostics);
            umi_language_runtime_json_writer_raw(&writer, ",\"triggerKind\":1}}");
            status = writer.status;
        }
    }
    UmiLanguageDiagnosticContextDestroy(context);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out_parameters = parameters;
    else
        free(parameters);
    return status;
}
#endif
/* Preserve the selected action family when fresh diagnostic context rebuilds request parameters in the same server connection.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryActionDiagnostics(UmiLanguageRuntimeServer *server,
                                        const UmiLanguageCompletionRequest *request,
                                        const SourceQueryPolicy *policy, CompletionQueryWire *wire,
                                        UmiClock *clock, uint64_t started, const UmiCancellationToken *cancel,
                                        UmiLanguageRuntimeEnvelope *reply, char **out_parameters)
{
    *out_parameters = NULL;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiLanguageDiagnosticContext *context = NULL;
    UmiStatus status =
        policy->pull_diagnostics
            ? QueryPullDiagnostics(server, request, wire, clock, started, cancel, reply, &catalogue)
            : QueryAwaitDiagnostics(server, request, clock, started, cancel, reply, &catalogue);
    const int32_t version = 1;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextCreate(
            catalogue, request->document_uri, &version, request->source, request->source_bytes,
            request->cursor_offset, policy->range_end, cancel, &context);
    const char *diagnostics = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextJson(context, &diagnostics, &bytes);
    (void)bytes;
    char *parameters = NULL;
    if (status == UMI_STATUS_OK)
    {
        parameters = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        if (parameters == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = request->source_bytes;
        view.capacity = request->source_bytes;
        UmiEditorTextPosition begin, end;
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &begin);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &end);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageRuntimeJsonWriter writer;
            umi_language_runtime_json_writer_init(&writer, parameters, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
            umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
            umi_language_runtime_json_writer_string(&writer, request->document_uri);
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":");
            umi_language_runtime_json_writer_raw(&writer, diagnostics);
            umi_language_runtime_json_writer_raw(&writer, ",\"triggerKind\":1}}");
            status = writer.status;
        }
    }
    UmiLanguageDiagnosticContextDestroy(context);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out_parameters = parameters;
    else
        free(parameters);
    return status;
}
#endif
static UmiStatus QueryActionDiagnostics(UmiLanguageRuntimeServer *server,
                                        const UmiLanguageCompletionRequest *request,
                                        const SourceQueryPolicy *policy, CompletionQueryWire *wire,
                                        UmiClock *clock, uint64_t started, const UmiCancellationToken *cancel,
                                        UmiLanguageRuntimeEnvelope *reply, char **out_parameters)
{
    *out_parameters = NULL;
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    UmiLanguageDiagnosticContext *context = NULL;
    UmiStatus status =
        policy->pull_diagnostics
            ? QueryPullDiagnostics(server, request, wire, clock, started, cancel, reply, &catalogue)
            : QueryAwaitDiagnostics(server, request, clock, started, cancel, reply, &catalogue);
    const int32_t version = 1;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextCreate(
            catalogue, request->document_uri, &version, request->source, request->source_bytes,
            request->cursor_offset, policy->range_end, cancel, &context);
    const char *diagnostics = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticContextJson(context, &diagnostics, &bytes);
    (void)bytes;
    char *parameters = NULL;
    if (status == UMI_STATUS_OK)
    {
        parameters = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        if (parameters == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = request->source_bytes;
        view.capacity = request->source_bytes;
        UmiEditorTextPosition begin, end;
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &begin);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &end);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageRuntimeJsonWriter writer;
            umi_language_runtime_json_writer_init(&writer, parameters, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
            umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
            umi_language_runtime_json_writer_string(&writer, request->document_uri);
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, begin.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":");
            umi_language_runtime_json_writer_raw(&writer, diagnostics);
            umi_language_runtime_json_writer_raw(&writer, ",\"triggerKind\":1");
            if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
            {
                umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
                umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
                umi_language_runtime_json_writer_raw(&writer, "]");
            }
            umi_language_runtime_json_writer_raw(&writer, "}}");
            status = writer.status;
        }
    }
    UmiLanguageDiagnosticContextDestroy(context);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out_parameters = parameters;
    else
        free(parameters);
    return status;
}
