/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_filters.inc
 * PURPOSE: Own exception-filter state alongside the active debugger session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after UmiDebugRuntimePlatform so configuration and inspection share
 * the same owner, transport and session lifetime. No independent adapter is created. */
static UmiStatus DebugExceptionInitialize(UmiDebugRuntimePlatform *platform, const char *json)
{
    if (platform->exceptions.selection.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDebugExceptionCatalog *catalog = malloc(sizeof *catalog);
    if (catalog == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDebugExceptionCatalogDecode(json, catalog);
    if (status == UMI_STATUS_OK)
    {
        uint64_t revision = platform->exceptions.selection.revision + 1U;
        memset(&platform->exceptions, 0, sizeof platform->exceptions);
        platform->exceptions.catalog = *catalog;
        platform->exceptions.selection.revision = revision;
        platform->exceptions.selection.count = catalog->count;
        for (size_t i = 0U; i < catalog->count; ++i)
            platform->exceptions.selection.enabled[i] = catalog->items[i].default_enabled;
    }
    free(catalog);
    return status;
}
static UmiStatus DebugExceptionConfigure(UmiDebugRuntimePlatform *platform,
                                         const UmiDebugExceptionSelection *selection,
                                         uint32_t timeout_ms)
{
    if (timeout_ms == 0U)
        return UMI_STATUS_TIMEOUT;
    if (platform->exceptions.catalog.count == 0U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (selection->count != platform->exceptions.catalog.count)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *filters[UMI_DEBUG_EXCEPTION_FILTER_LIMIT];
    size_t count = 0U;
    for (size_t i = 0U; i < selection->count; ++i)
    {
        if (selection->enabled[i] != 0 && selection->enabled[i] != 1)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (selection->enabled[i])
            filters[count++] = platform->exceptions.catalog.items[i].id;
    }
    if (platform->exceptions.selection.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDebugRuntimeEnvelope *reply = malloc(sizeof *reply);
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    /* Capture the requested set before writing. An error after transmission can
     * leave adapter state uncertain; it must not retain a previous success label. */
    uint64_t revision = platform->exceptions.selection.revision + 1U;
    platform->exceptions.selection = *selection;
    platform->exceptions.selection.revision = revision;
    snprintf(platform->exceptions.selection.session_id,
             sizeof platform->exceptions.selection.session_id, "%s", platform->active_session_id);
    platform->exceptions.acknowledged = 0;
    memset(&platform->exceptions.acknowledgement, 0, sizeof platform->exceptions.acknowledgement);
    platform->exceptions.acknowledgement.count = count;
    for (size_t i = 0U; i < UMI_DEBUG_EXCEPTION_FILTER_LIMIT; ++i)
        platform->exceptions.acknowledgement.verified[i] = -1;
    uint64_t sequence = 0U;
    UmiStatus status = umi_debug_runtime_request_set_exception_breakpoints(
        platform->adapter, filters, count, &sequence);
    if (status == UMI_STATUS_OK)
        status =
            umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, reply);
    if (status == UMI_STATUS_OK)
        status = UmiDebugExceptionAcknowledgementDecode(reply->json, count,
                                                        &platform->exceptions.acknowledgement);
    platform->exceptions.acknowledged = status == UMI_STATUS_OK;
    free(reply);
    return status;
}
/* Retain the former signature for engineering review. The shared implementation
 * below copies the same session-owned filter aggregate using its distinct public
 * type, preserving session checks, field layout and the callable API name. */
#if 0
UmiStatus UmiDebugRuntimeExceptionFiltersRead(const UmiDebugRuntimePlatform *platform,
                                              UmiDebugExceptionSnapshot *out)
#endif
UmiStatus UmiDebugRuntimeExceptionFiltersRead(const UmiDebugRuntimePlatform *platform,
                                              UmiDebugExceptionFiltersSnapshot *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = platform->exceptions;
    /* A filter-less session still carries its identity for presentation. */
    snprintf(out->selection.session_id, sizeof out->selection.session_id, "%s",
             platform->active_session_id);
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeExceptionFiltersApply(UmiDebugRuntimePlatform *platform,
                                               const UmiDebugExceptionSelection *selection,
                                               uint32_t timeout_ms)
{
    if (platform == NULL || selection == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (memchr(selection->session_id, '\0', sizeof selection->session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(selection->session_id, platform->active_session_id) != 0 ||
        selection->revision != platform->exceptions.selection.revision)
        return UMI_STATUS_INVALID_STATE;
    return DebugExceptionConfigure(platform, selection, timeout_ms);
}
