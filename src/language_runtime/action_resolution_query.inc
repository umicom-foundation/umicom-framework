/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/action_resolution_query.inc
 * PURPOSE: Resolve bounded complete text-action proposals within the source query that produced their opaque data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus QueryResolveActions(UmiLanguageRuntimeServer *server,
                                     const UmiLanguageCompletionRequest *request,
                                     const SourceQueryPolicy *policy, UmiClock *clock, uint64_t started,
                                     const UmiCancellationToken *cancel, UmiLanguageRuntimeEnvelope *reply,
                                     UmiLanguageCodeActionCatalogue **catalogue)
{
    size_t count = UmiLanguageCodeActionCatalogueCount(*catalogue), eligible = 0U;
    UmiStatus status = UMI_STATUS_OK;
    /* Preflight all candidates before contacting the resolver. A limited
     * request must not silently resolve the first few and omit the rest. */
    for (size_t i = 0U; i < count; ++i)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        UmiLanguageCodeAction action;
        status = UmiLanguageCodeActionCatalogueAt(*catalogue, i, &action);
        if (status != UMI_STATUS_OK)
            return status;
        if (!action.disabled && !action.has_edit && !action.has_command)
            ++eligible;
    }
    if (eligible > policy->resolve_action_limit)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        UmiLanguageCodeAction action;
        status = UmiLanguageCodeActionCatalogueAt(*catalogue, i, &action);
        if (status != UMI_STATUS_OK)
            break;
        if (action.disabled || action.has_edit || action.has_command)
            continue;
        const char *span = NULL;
        size_t bytes = 0U;
        status = UmiLanguageCodeActionCatalogueItemJson(*catalogue, i, &span, &bytes);
        if (status == UMI_STATUS_OK && bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        char *parameters = status == UMI_STATUS_OK ? malloc(bytes + 1U) : NULL;
        if (status == UMI_STATUS_OK && parameters == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        uint64_t id = 0U;
        if (status == UMI_STATUS_OK)
        {
            /* The owned request is a full original action, not reconstructed
             * title/data fields. No server identity crosses a closed session. */
            memcpy(parameters, span, bytes);
            parameters[bytes] = '\0';
            status = umi_language_runtime_server_send_request(server, "codeAction/resolve", parameters,
                                                              "source-action-resolution", &id);
        }
        free(parameters);
        if (status == UMI_STATUS_OK)
            status = QueryAwait(server, id, clock, started, request->timeout_ms, cancel, reply);
        UmiLanguageCodeActionCatalogue *resolved = NULL;
        if (status == UMI_STATUS_OK)
            status = UmiLanguageCodeActionCatalogueWithResolvedResponse(
                *catalogue, i, reply->json, strlen(reply->json), id, cancel, &resolved);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(*catalogue);
            *catalogue = resolved;
        }
    }
    return status;
}
