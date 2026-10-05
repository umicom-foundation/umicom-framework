/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/platform_context.inc
 * PURPOSE: Keep asynchronous language replies attached to their server and document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* These helpers share the platform's owning thread. The server manager retains
 * every connection until platform destruction, so stored pointers are stable. */
static UmiStatus PlatformContextReady(const UmiLanguageRuntimePlatform *p,
                                      const UmiLanguageRuntimeServer *server)
{
    if (p == NULL || server == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (p->ctx.active_count >= UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS || p->ctx.sequence == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
static void PlatformContextErase(UmiLanguageRuntimePlatform *p, size_t index)
{
    memset(&p->ctx.items[index], 0, sizeof(p->ctx.items[index]));
    p->context_servers[index] = NULL;
    p->context_versions[index] = 0U;
    p->context_buffer_revisions[index] = 0U;
    --p->ctx.active_count;
}
static void PlatformForgetDocument(UmiLanguageRuntimePlatform *p, const char *document)
{
    for (size_t i = 0U; i < UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS; ++i)
        if (p->ctx.items[i].active && strcmp(p->ctx.items[i].document_id, document) == 0)
            PlatformContextErase(p, i);
    if (p->completion != NULL && strcmp(p->completion_context.document_id, document) == 0)
    {
        UmiLanguageCompletionCatalogueDestroy(p->completion);
        p->completion = NULL;
    }
    /* Retired replies may still arrive. The server consumes their pending
     * protocol IDs, while the platform refuses to publish their old context. */
}
static UmiStatus PlatformContextTake(UmiLanguageRuntimePlatform *p, UmiLanguageRuntimeServer *server,
                                     uint64_t request_id, UmiLanguageRuntimeRequestContext *out,
                                     uint64_t *version, uint64_t *buffer_revision)
{
    for (size_t i = 0U; i < UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS; ++i)
    {
        if (p->ctx.items[i].active && p->context_servers[i] == server &&
            p->ctx.items[i].request_id == request_id)
        {
            *out = p->ctx.items[i];
            *version = p->context_versions[i];
            *buffer_revision = p->context_buffer_revisions[i];
            PlatformContextErase(p, i);
            /* Once a newer answer arrives, older requests for that operation
             * cannot become current again even after this slot is reused. */
            for (size_t older = 0U; older < UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS; ++older)
            {
                UmiLanguageRuntimeRequestContext *candidate = &p->ctx.items[older];
                if (candidate->active && p->context_servers[older] == server &&
                    candidate->sequence < out->sequence &&
                    strcmp(candidate->document_id, out->document_id) == 0 &&
                    strcmp(candidate->method, out->method) == 0)
                    PlatformContextErase(p, older);
            }
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
static int PlatformContextCurrent(UmiLanguageRuntimePlatform *p, UmiLanguageRuntimeServer *server,
                                  const UmiLanguageRuntimeRequestContext *request, uint64_t version)
{
    if (request->document_id[0] != '\0')
    {
        UmiLanguageRuntimeDocumentState document;
        if (umi_language_runtime_document_find(p->d, request->document_id, &document) != UMI_STATUS_OK ||
            !document.open || document.server != server || document.version != version ||
            strcmp(document.uri, request->uri) != 0)
            return 0;
    }
    /* Prefer the most recently requested answer for a given operation. An
     * older reply must not replace a newer cursor-position result. */
    for (size_t i = 0U; i < UMI_LANGUAGE_RUNTIME_MAX_PENDING_REQUESTS; ++i)
    {
        const UmiLanguageRuntimeRequestContext *candidate = &p->ctx.items[i];
        if (candidate->active && p->context_servers[i] == server && candidate->sequence > request->sequence &&
            strcmp(candidate->document_id, request->document_id) == 0 &&
            strcmp(candidate->method, request->method) == 0)
            return 0;
    }
    if (p->completion != NULL && strcmp(request->method, "textDocument/completion") == 0 &&
        strcmp(p->completion_context.document_id, request->document_id) == 0 &&
        p->completion_context.sequence > request->sequence)
        return 0;
    return 1;
}
static UmiStatus PlatformNotificationAllowed(UmiLanguageRuntimePlatform *p, UmiLanguageRuntimeServer *server,
                                             const UmiLanguageRuntimeEnvelope *envelope, int *allowed)
{
    *allowed = 1;
    if (strcmp(envelope->method, "textDocument/publishDiagnostics") != 0)
        return UMI_STATUS_OK;
    *allowed = 0;
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(envelope->json, strlen(envelope->json), NULL, NULL, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    int params, uri, version;
    status = UmiJsonTreeMember(tree, 0, "params", &params);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, params, "uri", &uri);
    char text[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, uri, text, sizeof(text));
    UmiLanguageRuntimeDocumentState document;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_document_find_by_uri(p->d, text, &document);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        status = UMI_STATUS_OK;
        goto done;
    }
    if (status != UMI_STATUS_OK || !document.open || document.server != server)
        goto done;
    status = UmiJsonTreeMember(tree, params, "version", &version);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        status = UMI_STATUS_OK;
        *allowed = 1;
    }
    else if (status == UMI_STATUS_OK)
    {
        int64_t published;
        status = UmiJsonTreeInteger(tree, version, &published);
        if (status == UMI_STATUS_OK)
            *allowed = published >= 0 && (uint64_t)published == document.version;
    }
done:
    UmiJsonTreeDestroy(tree);
    return status;
}
