/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/exception_inspection.inc
 * PURPOSE: Tie exception metadata retrieval to a current observed exception stop.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDebugRuntimeExceptionTargetRead(const UmiDebugRuntimePlatform *platform,
                                             UmiDebugExceptionTarget *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugExceptionTarget target = {0};
    UmiStatus status = UmiDebugRuntimeConnectionIdentityRead(platform, &target.connection);
    if (status != UMI_STATUS_OK)
        return status;
    if (!platform->capabilities.supports_exception_info_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (!platform->paused || !platform->stopped_by_exception || platform->active_thread_id == 0U ||
        platform->active_thread_id > INT32_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK)
        return status;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)
        return UMI_STATUS_INVALID_STATE;
    target.revision = platform->revision;
    target.thread_id = (uint32_t)platform->active_thread_id;
    *out = target;
    return UMI_STATUS_OK;
}
static UmiStatus ExceptionTargetCheck(const UmiDebugRuntimePlatform *platform,
                                      const UmiDebugExceptionTarget *expected)
{
    if (expected == NULL || memchr(expected->connection.session_id, '\0',
                                   sizeof expected->connection.session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugExceptionTarget current;
    UmiStatus status = UmiDebugRuntimeExceptionTargetRead(platform, &current);
    if (status != UMI_STATUS_OK)
        return status;
    return current.connection.generation == expected->connection.generation &&
                   strcmp(current.connection.session_id, expected->connection.session_id) == 0 &&
                   current.revision == expected->revision &&
                   current.thread_id == expected->thread_id
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_STATE;
}
UmiStatus UmiDebugRuntimeExceptionInformationRead(UmiDebugRuntimePlatform *platform,
                                                  const UmiDebugExceptionTarget *target,
                                                  uint32_t timeout_ms,
                                                  UmiDebugExceptionInformation *out)
{
    if (platform == NULL || out == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ExceptionTargetCheck(platform, target);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugExceptionInformation *result = malloc(sizeof *result);
    if (response == NULL || result == NULL)
    {
        free(response);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    status =
        umi_debug_runtime_request_exception_info(platform->adapter, target->thread_id, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugExceptionInformationDecode(response->json, result);
    /* A reply can race a continue or new stop. Publish only after proving the
     * selected thread and complete inspection revision still match the capture. */
    if (status == UMI_STATUS_OK)
        status = ExceptionTargetCheck(platform, target);
    if (status == UMI_STATUS_OK)
        *out = *result;
    free(result);
    free(response);
    return status;
}
