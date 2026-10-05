/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/call_query_support.inc
 * PURPOSE: Keep prepared call identities and every direct relationship inside one source-query lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

struct UmiLanguageCallResult
{
    UmiLanguageCallItemCatalogue *roots;
    UmiLanguageCallEdgeCatalogue *edges[16];
    size_t count, sites;
};
void UmiLanguageCallResultDestroy(UmiLanguageCallResult *result)
{
    if (result == NULL)
        return;
    for (size_t i = 0U; i < 16U; ++i)
        UmiLanguageCallEdgeCatalogueDestroy(result->edges[i]);
    UmiLanguageCallItemCatalogueDestroy(result->roots);
    free(result);
}
size_t UmiLanguageCallResultRootCount(const UmiLanguageCallResult *result)
{
    return result == NULL ? 0U : UmiLanguageCallItemCatalogueCount(result->roots);
}
size_t UmiLanguageCallResultCount(const UmiLanguageCallResult *result)
{
    return result == NULL ? 0U : result->count;
}
UmiStatus UmiLanguageCallResultRootAt(const UmiLanguageCallResult *result, size_t index,
                                      UmiLanguageCallItem *out)
{
    if (result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiLanguageCallItemCatalogueAt(result->roots, index, out);
}
static UmiStatus CallResultFind(const UmiLanguageCallResult *result, size_t index, size_t *root, size_t *edge)
{
    if (result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= result->count)
        return UMI_STATUS_NOT_FOUND;
    for (size_t i = 0U; i < UmiLanguageCallResultRootCount(result); ++i)
    {
        size_t count = UmiLanguageCallEdgeCatalogueCount(result->edges[i]);
        if (index < count)
        {
            *root = i;
            *edge = index;
            return UMI_STATUS_OK;
        }
        index -= count;
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiLanguageCallResultAt(const UmiLanguageCallResult *result, size_t index,
                                  UmiLanguageCallRelation *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t root = 0U, edge = 0U;
    UmiStatus status = CallResultFind(result, index, &root, &edge);
    UmiLanguageCallRelation value = {0};
    UmiLanguageCallEdge related;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCallItemCatalogueAt(result->roots, root, &value.root);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCallEdgeCatalogueAt(result->edges[root], edge, &related);
    if (status == UMI_STATUS_OK)
    {
        value.root_index = root;
        value.related = related.related;
        value.call_sites = related.call_sites;
        *out = value;
    }
    return status;
}
UmiStatus UmiLanguageCallResultLocation(const UmiLanguageCallResult *result, size_t index, size_t destination,
                                        UmiLanguageSourceLocation *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t root = 0U, edge = 0U;
    UmiStatus status = CallResultFind(result, index, &root, &edge);
    return status == UMI_STATUS_OK
               ? UmiLanguageCallEdgeCatalogueLocation(result->edges[root], edge, destination, out)
               : status;
}
static UmiStatus QueryCallLocation(const UmiLanguageCompletionRequest *request,
                                   const UmiLanguageSourceLocation *location,
                                   const UmiCancellationToken *cancel, UmiEditorTextPositionIndex **index)
{
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (strcmp(location->uri, request->document_uri) != 0)
        return UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
    if (*index == NULL)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = view.capacity = request->source_bytes;
        status = UmiEditorTextPositionIndexCreate(&view, cancel, index);
    }
    if (status == UMI_STATUS_OK)
        status = QueryLocationRange(*index, location->target);
    if (status == UMI_STATUS_OK)
        status = QueryLocationRange(*index, location->selection);
    return status;
}
static UmiStatus QueryCallHierarchy(UmiLanguageRuntimeServer *server,
                                    const UmiLanguageCompletionRequest *request,
                                    const SourceQueryPolicy *policy, UmiClock *clock, uint64_t started,
                                    uint64_t prepared_id, const UmiCancellationToken *cancel,
                                    UmiLanguageRuntimeEnvelope *reply, UmiLanguageCallResult **out)
{
    *out = NULL;
    UmiLanguageCallResult *result = calloc(1U, sizeof(*result));
    if (result == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiLanguageCallItemCatalogueReadResponse(reply->json, strlen(reply->json), prepared_id,
                                                                cancel, &result->roots);
    size_t roots = UmiLanguageCallResultRootCount(result);
    if (status == UMI_STATUS_OK && roots > 16U)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorTextPositionIndex *index = NULL;
    /* Validate every prepared root before issuing any follow-up request. The
     * language server may prepare a definition in another document; do not
     * assume it shares the source URI or contains the requested caret. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < roots; ++i)
    {
        UmiLanguageCallItem root;
        status = UmiLanguageCallItemCatalogueAt(result->roots, i, &root);
        if (status == UMI_STATUS_OK)
            status = QueryCallLocation(request, &root.location, cancel, &index);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < roots; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const char *item = NULL;
        size_t bytes = 0U;
        uint64_t id = 0U;
        status = UmiLanguageCallItemCatalogueItemJson(result->roots, i, &item, &bytes);
        if (status == UMI_STATUS_OK && bytes > UMI_LANGUAGE_RUNTIME_JSON_CAPACITY - 10U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        char *parameters = status == UMI_STATUS_OK ? malloc(bytes + 10U) : NULL;
        if (status == UMI_STATUS_OK && parameters == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
        {
            /* Return the complete immutable item to its original session;
             * opaque server data must never be reconstructed from display text. */
            memcpy(parameters, "{\"item\":", 8U);
            memcpy(parameters + 8U, item, bytes);
            parameters[8U + bytes] = '}';
            parameters[9U + bytes] = '\0';
            status = umi_language_runtime_server_send_request(
                server,
                policy->call_direction == UMI_LANGUAGE_CALL_INCOMING ? "callHierarchy/incomingCalls"
                                                                     : "callHierarchy/outgoingCalls",
                parameters, "source-call-relationships", &id);
        }
        free(parameters);
        if (status == UMI_STATUS_OK)
            status = QueryAwait(server, id, clock, started, request->timeout_ms, cancel, reply);
        UmiLanguageCallItem root;
        if (status == UMI_STATUS_OK)
            status = UmiLanguageCallItemCatalogueAt(result->roots, i, &root);
        if (status == UMI_STATUS_OK)
            status =
                UmiLanguageCallEdgeCatalogueReadResponse(reply->json, strlen(reply->json), id, &root,
                                                         policy->call_direction, cancel, &result->edges[i]);
        size_t edges = UmiLanguageCallEdgeCatalogueCount(result->edges[i]);
        if (status == UMI_STATUS_OK && edges > 4096U - result->count)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t j = 0U; status == UMI_STATUS_OK && j < edges; ++j)
        {
            UmiLanguageCallEdge edge;
            status = UmiLanguageCallEdgeCatalogueAt(result->edges[i], j, &edge);
            if (status == UMI_STATUS_OK && edge.call_sites > 16384U - result->sites)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            if (status != UMI_STATUS_OK)
                break;
            result->sites += edge.call_sites;
            for (size_t k = 0U; status == UMI_STATUS_OK && k <= edge.call_sites; ++k)
            {
                UmiLanguageSourceLocation location;
                status = UmiLanguageCallEdgeCatalogueLocation(result->edges[i], j, k, &location);
                if (status == UMI_STATUS_OK)
                    status = QueryCallLocation(request, &location, cancel, &index);
            }
        }
        if (status == UMI_STATUS_OK)
            result->count += edges;
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = result;
    else
        UmiLanguageCallResultDestroy(result);
    return status;
}
