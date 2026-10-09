/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/source_catalog.inc
 * PURPOSE: Request source metadata and text through the already-owned native debugger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDebugRuntimeSourceCatalogRead(UmiDebugRuntimePlatform *platform, uint32_t timeout_ms,
                                           UmiDebugSourceCatalog *out)
{
    if (platform == NULL || out == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugConnectionIdentity connection;
    UmiStatus status = UmiDebugRuntimeConnectionIdentityRead(platform, &connection);
    if (status != UMI_STATUS_OK)
        return status;
    if (!platform->capabilities.supports_loaded_sources_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugSourceCatalog *catalog = malloc(sizeof *catalog);
    if (response == NULL || catalog == NULL)
    {
        free(response);
        free(catalog);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_loaded_sources(platform->adapter, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSourceCatalogDecode(response->json, catalog);
    if (status == UMI_STATUS_OK)
        status = DebugConnectionIdentityCheck(platform, &connection);
    if (status == UMI_STATUS_OK)
    {
        catalog->connection = connection;
        *out = *catalog;
    }
    free(catalog);
    free(response);
    return status;
}
UmiStatus UmiDebugRuntimeSourceContentRead(UmiDebugRuntimePlatform *platform,
                                           const UmiDebugConnectionIdentity *connection,
                                           uint32_t reference, uint32_t timeout_ms,
                                           UmiDebugSourceContent *out)
{
    if (platform == NULL || out == NULL || reference == 0U || reference > INT32_MAX ||
        timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DebugConnectionIdentityCheck(platform, connection);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugSourceContent *content = malloc(sizeof *content);
    if (response == NULL || content == NULL)
    {
        free(response);
        free(content);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* A positive reference belongs to the adapter. Omitting the path prevents
     * accidental local-file fallback when inspecting generated or remote source. */
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_source(platform->adapter, NULL, reference, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSourceContentDecode(response->json, content);
    if (status == UMI_STATUS_OK)
        status = DebugConnectionIdentityCheck(platform, connection);
    if (status == UMI_STATUS_OK)
        *out = *content;
    free(content);
    free(response);
    return status;
}
