/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_pull_query.inc
 * PURPOSE: Negotiate an explicit diagnostic provider and validate its complete captured-source response.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Provider identifiers select a diagnostic service, not a command. Decode and
 * escape the identifier as data, then keep the request in the existing bounded
 * wire owner. Source is not opened until capability negotiation succeeds. */
static UmiStatus QueryPullDiagnosticPrepare(const char *json, uint64_t request_id,
                                            const UmiLanguageCompletionRequest *request,
                                            CompletionQueryWire *wire)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities = -1, provider = -1, identifier = -1;
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "diagnosticProvider", &provider);
    if (status == UMI_STATUS_OK && provider < 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, provider) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    const char *required[] = {"interFileDependencies", "workspaceDiagnostics"};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 2U; ++i)
    {
        int field = -1, enabled = 0;
        status = CompletionMember(tree, provider, required[i], &field);
        if (status == UMI_STATUS_OK && field < 0)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeBoolean(tree, field, &enabled);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, provider, "identifier", &identifier);
    char identity[4097];
    if (status == UMI_STATUS_OK && identifier >= 0)
        status = CompletionText(tree, identifier, identity, sizeof(identity));
    if (status == UMI_STATUS_OK)
    {
        UmiLanguageRuntimeJsonWriter writer;
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "}");
        if (identifier >= 0)
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"identifier\":");
            umi_language_runtime_json_writer_string(&writer, identity);
        }
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
/* Pull results have no document version field. Freshness follows the exclusive
 * session and the exact draft sent in didOpen; the owner rechecks it again
 * before presenting or navigating a returned diagnostic. */
static UmiStatus QueryPullDiagnostics(UmiLanguageRuntimeServer *server,
                                      const UmiLanguageCompletionRequest *request, CompletionQueryWire *wire,
                                      UmiClock *clock, uint64_t started, const UmiCancellationToken *cancel,
                                      UmiLanguageRuntimeEnvelope *reply, UmiLanguageDiagnosticCatalogue **out)
{
    *out = NULL;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    uint64_t id = 0U;
    UmiStatus status = umi_language_runtime_server_send_request(
        server, "textDocument/diagnostic", wire->position, "source-diagnostics-pull", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, clock, started, request->timeout_ms, cancel, reply);
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticCatalogueReadPullResponse(reply->json, strlen(reply->json), id,
                                                                request->document_uri, cancel, &catalogue);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticCatalogueValidateSource(catalogue, request->document_uri, NULL,
                                                              request->source, request->source_bytes, cancel);
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return status;
}
