/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/module_page.inc
 * PURPOSE: Read one module page without replacing the canonical registry with a partial list.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Module pages use the shared connection generation so source and module inspection follow the same reconnect boundary. The prior implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeModuleSessionRead(const UmiDebugRuntimePlatform *platform,
                                           UmiDebugModuleSession *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugModuleSession session = {0};
    snprintf(session.session_id, sizeof session.session_id, "%s", platform->active_session_id);
    session.generation = platform->module_generation;
    session.supported = platform->capabilities.supports_modules_request;
    *out = session;
    return UMI_STATUS_OK;
}
#endif
/* Module session capture now uses the same lifecycle-event gate as source inspection. The earlier local active-state check is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeModuleSessionRead(const UmiDebugRuntimePlatform *platform,
                                           UmiDebugModuleSession *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugModuleSession session = {0};
    snprintf(session.session_id, sizeof session.session_id, "%s", platform->active_session_id);
    session.generation = platform->inspection_generation;
    session.supported = platform->capabilities.supports_modules_request;
    *out = session;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDebugRuntimeModuleSessionRead(const UmiDebugRuntimePlatform *platform,
                                           UmiDebugModuleSession *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugConnectionIdentity identity;
    UmiStatus status=UmiDebugRuntimeConnectionIdentityRead(platform,&identity);
    if(status!=UMI_STATUS_OK)return status;
    UmiDebugModuleSession session = {0};
    snprintf(session.session_id, sizeof session.session_id, "%s", platform->active_session_id);
    session.generation = platform->inspection_generation;
    session.supported = platform->capabilities.supports_modules_request;
    *out = session;
    return UMI_STATUS_OK;
}
/* Module pages use the shared connection generation so source and module inspection follow the same reconnect boundary. The prior implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeModulePageRead(UmiDebugRuntimePlatform *platform,
                                        const UmiDebugModuleSession *session, uint32_t first,
                                        uint32_t requested_count, uint32_t timeout_ms,
                                        UmiDebugModulePage *out)
{
    if (platform == NULL || session == NULL || out == NULL || timeout_ms == 0U ||
        first > INT32_MAX || requested_count == 0U || requested_count > UMI_DEBUG_MODULE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (memchr(session->session_id, '\0', sizeof session->session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generation != platform->module_generation ||
        strcmp(session->session_id, platform->active_session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_modules_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugModulePage *page = malloc(sizeof *page);
    if (response == NULL || page == NULL)
    {
        free(response);
        free(page);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    UmiStatus status =
        umi_debug_runtime_request_modules(platform->adapter, first, requested_count, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugModulePageDecode(response->json, first, requested_count, page);
    if (status == UMI_STATUS_OK)
    {
        page->session = *session;
        page->session.supported = 1;
        *out = *page;
    }
    free(page);
    free(response);
    return status;
}
#endif
/* Module publication now checks the captured connection before and after adapter waiting, because buffered lifecycle events can invalidate a reply. The previous reader is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeModulePageRead(UmiDebugRuntimePlatform *platform,
                                        const UmiDebugModuleSession *session, uint32_t first,
                                        uint32_t requested_count, uint32_t timeout_ms,
                                        UmiDebugModulePage *out)
{
    if (platform == NULL || session == NULL || out == NULL || timeout_ms == 0U ||
        first > INT32_MAX || requested_count == 0U || requested_count > UMI_DEBUG_MODULE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (memchr(session->session_id, '\0', sizeof session->session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generation != platform->inspection_generation ||
        strcmp(session->session_id, platform->active_session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_modules_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugModulePage *page = malloc(sizeof *page);
    if (response == NULL || page == NULL)
    {
        free(response);
        free(page);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    UmiStatus status =
        umi_debug_runtime_request_modules(platform->adapter, first, requested_count, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugModulePageDecode(response->json, first, requested_count, page);
    if (status == UMI_STATUS_OK)
    {
        page->session = *session;
        page->session.supported = 1;
        *out = *page;
    }
    free(page);
    free(response);
    return status;
}
#endif
UmiStatus UmiDebugRuntimeModulePageRead(UmiDebugRuntimePlatform *platform,
                                        const UmiDebugModuleSession *session, uint32_t first,
                                        uint32_t requested_count, uint32_t timeout_ms,
                                        UmiDebugModulePage *out)
{
    if (platform == NULL || session == NULL || out == NULL || timeout_ms == 0U ||
        first > INT32_MAX || requested_count == 0U || requested_count > UMI_DEBUG_MODULE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (memchr(session->session_id, '\0', sizeof session->session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generation != platform->inspection_generation ||
        strcmp(session->session_id, platform->active_session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_modules_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDebugConnectionIdentity identity;
    memcpy(identity.session_id,session->session_id,sizeof identity.session_id);
    identity.generation=session->generation;
    UmiStatus identity_status=DebugConnectionIdentityCheck(platform,&identity);
    if(identity_status!=UMI_STATUS_OK)return identity_status;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugModulePage *page = malloc(sizeof *page);
    if (response == NULL || page == NULL)
    {
        free(response);
        free(page);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    UmiStatus status =
        umi_debug_runtime_request_modules(platform->adapter, first, requested_count, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugModulePageDecode(response->json, first, requested_count, page);
    if(status==UMI_STATUS_OK)status=DebugConnectionIdentityCheck(platform,&identity);
    if (status == UMI_STATUS_OK)
    {
        page->session = *session;
        page->session.supported = 1;
        *out = *page;
    }
    free(page);
    free(response);
    return status;
}
