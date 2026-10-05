/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_query_support.inc
 * PURPOSE: Wait for a complete source-matched publication without issuing a fictitious diagnostic request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Diagnostics arrive as notifications rather than request replies. Keep the
 * same bounded read loop and reject server requests we cannot answer. The
 * exclusive source owner determines freshness; no shared global list is read. */
static UmiStatus QueryAwaitDiagnostics(UmiLanguageRuntimeServer *server,
                                       const UmiLanguageCompletionRequest *request, UmiClock *clock,
                                       uint64_t started, const UmiCancellationToken *cancel,
                                       UmiLanguageRuntimeEnvelope *reply,
                                       UmiLanguageDiagnosticCatalogue **out)
{
    unsigned messages = 0U;
    for (;;)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        uint64_t elapsed = (clock->monotonic_nanoseconds(clock) - started) / 1000000U;
        if (request->timeout_ms != 0U && elapsed >= request->timeout_ms)
            return UMI_STATUS_TIMEOUT;
        uint32_t remaining = elapsed >= request->timeout_ms ? 0U : request->timeout_ms - (uint32_t)elapsed;
        UmiStatus status =
            umi_language_runtime_server_receive(server, remaining > 50U ? 50U : remaining, reply);
        if (status == UMI_STATUS_OK)
        {
            if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST)
                return UMI_STATUS_NOT_IMPLEMENTED;
            if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_NOTIFICATION &&
                strcmp(reply->method, "textDocument/publishDiagnostics") == 0)
            {
                UmiLanguageDiagnosticCatalogue *catalogue = NULL;
                status = UmiLanguageDiagnosticCatalogueReadNotification(reply->json, strlen(reply->json),
                                                                        cancel, &catalogue);
                UmiLanguageDiagnosticPublication publication;
                if (status == UMI_STATUS_OK)
                    status = UmiLanguageDiagnosticCataloguePublication(catalogue, &publication);
                if (status == UMI_STATUS_OK && strcmp(publication.uri, request->document_uri) == 0 &&
                    (!publication.has_version || publication.version == 1))
                {
                    const int32_t version = 1;
                    status = UmiLanguageDiagnosticCatalogueValidateSource(catalogue, request->document_uri,
                                                                          &version, request->source,
                                                                          request->source_bytes, cancel);
                    if (status == UMI_STATUS_OK)
                    {
                        *out = catalogue;
                        return UMI_STATUS_OK;
                    }
                }
                UmiLanguageDiagnosticCatalogueDestroy(catalogue);
                if (status != UMI_STATUS_OK)
                    return status;
            }
            if (++messages >= 4096U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (status != UMI_STATUS_NOT_FOUND)
            return status;
        if (!umi_language_runtime_server_is_running(server))
            return UMI_STATUS_UNAVAILABLE;
        if ((clock->monotonic_nanoseconds(clock) - started) / 1000000U >= request->timeout_ms)
            return UMI_STATUS_TIMEOUT;
        if (status == UMI_STATUS_NOT_FOUND)
            (void)clock->sleep_milliseconds(clock, 1U);
    }
}
