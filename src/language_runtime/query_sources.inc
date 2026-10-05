/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/query_sources.inc
 * PURPOSE: Prepare bounded additional draft notifications and validate returned captured-file edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void QueryWireDestroy(CompletionQueryWire *wire)
{
    if (wire == NULL)
        return;
    free(wire->additional);
    free(wire);
}
static UmiStatus QueryPrepareSources(const UmiLanguageCompletionRequest *request,
                                     const SourceQueryPolicy *policy, const UmiCancellationToken *cancel,
                                     CompletionQueryWire *wire)
{
    if (policy->source_count == 0U)
        return UMI_STATUS_OK;
    if (policy->source_count >= UMI_LANGUAGE_SOURCE_QUERY_MAXIMUM_DOCUMENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (policy->additional_sources == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < policy->source_count; ++i)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        const UmiLanguageQuerySource *source = &policy->additional_sources[i];
        if (source->document_uri == NULL || source->language_id == NULL || source->source == NULL ||
            source->document_uri[0] == '\0' || source->language_id[0] == '\0')
            return UMI_STATUS_INVALID_ARGUMENT;
        if (source->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
            strlen(source->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
            strlen(source->language_id) >= 128U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus status = QueryText(source->source, source->source_bytes);
        if (status == UMI_STATUS_OK)
            status = QueryText(source->document_uri, strlen(source->document_uri));
        if (status == UMI_STATUS_OK)
            status = QueryText(source->language_id, strlen(source->language_id));
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(source->document_uri, request->document_uri) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
        for (size_t prior = 0U; prior < i; ++prior)
            if (strcmp(source->document_uri, policy->additional_sources[prior].document_uri) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    wire->additional = calloc(policy->source_count, sizeof(*wire->additional));
    if (wire->additional == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    wire->additional_count = policy->source_count;
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (envelope == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < policy->source_count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const UmiLanguageQuerySource *source = &policy->additional_sources[i];
        char *copy = malloc(source->source_bytes + 1U);
        if (copy == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        memcpy(copy, source->source, source->source_bytes);
        copy[source->source_bytes] = '\0';
        UmiLanguageRuntimeJsonWriter writer;
        SourceQueryDocumentWire *document = &wire->additional[i];
        umi_language_runtime_json_writer_init(&writer, document->open, sizeof(document->open));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, source->document_uri);
        umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
        umi_language_runtime_json_writer_string(&writer, source->language_id);
        umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
        umi_language_runtime_json_writer_string(&writer, copy);
        umi_language_runtime_json_writer_raw(&writer, "}}");
        status = writer.status;
        free(copy);
        if (status == UMI_STATUS_OK)
            status = umi_language_runtime_build_notification("textDocument/didOpen", document->open, envelope,
                                                             UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        umi_language_runtime_json_writer_init(&writer, document->close, sizeof(document->close));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, source->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "}}");
        if (status == UMI_STATUS_OK)
            status = writer.status;
        if (status == UMI_STATUS_OK)
            status = umi_language_runtime_build_notification("textDocument/didClose", document->close,
                                                             envelope, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    }
    free(envelope);
    return status;
}

/* Returned edits for supplied drafts must use the source actually opened.
 * Unknown targets remain in the complete catalogue for later resolution;
 * callers cannot treat them as an automatically applicable partial result. */
/* Use the shared workspace-preview contract with the caret before the known document version; retain the initial call for review.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCheckSourceEdits(const UmiLanguageWorkspaceEditCatalogue *catalogue,
    const SourceQueryPolicy *policy,const UmiCancellationToken *cancel)
{
    for(size_t i=0U;i<UmiLanguageWorkspaceEditCatalogueCount(catalogue);++i) {
        UmiLanguageWorkspaceDocumentChange change;
        UmiStatus status=UmiLanguageWorkspaceEditCatalogueDocument(catalogue,i,&change);
        if(status!=UMI_STATUS_OK) return status;
        for(size_t j=0U;j<policy->source_count;++j) {
            const UmiLanguageQuerySource *source=&policy->additional_sources[j];
            if(strcmp(change.uri,source->document_uri)!=0) continue;
            int32_t version=1;UmiLanguageTextEditPreview *preview=NULL;
            status=UmiLanguageWorkspaceEditCataloguePreview(catalogue,i,source->source,source->source_bytes,
                &version,0U,cancel,&preview);
            UmiLanguageTextEditPreviewDestroy(preview);
            if(status!=UMI_STATUS_OK) return status;
            break;
        }
    }
    return UMI_STATUS_OK;
}
#endif
static UmiStatus QueryCheckSourceEdits(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                       const SourceQueryPolicy *policy, const UmiCancellationToken *cancel)
{
    for (size_t i = 0U; i < UmiLanguageWorkspaceEditCatalogueCount(catalogue); ++i)
    {
        UmiLanguageWorkspaceDocumentChange change;
        UmiStatus status = UmiLanguageWorkspaceEditCatalogueDocument(catalogue, i, &change);
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t j = 0U; j < policy->source_count; ++j)
        {
            const UmiLanguageQuerySource *source = &policy->additional_sources[j];
            if (strcmp(change.uri, source->document_uri) != 0)
                continue;
            int32_t version = 1;
            UmiLanguageTextEditPreview *preview = NULL;
            status = UmiLanguageWorkspaceEditCataloguePreview(
                catalogue, i, source->source, source->source_bytes, 0U, &version, cancel, &preview);
            UmiLanguageTextEditPreviewDestroy(preview);
            if (status != UMI_STATUS_OK)
                return status;
            break;
        }
    }
    return UMI_STATUS_OK;
}
