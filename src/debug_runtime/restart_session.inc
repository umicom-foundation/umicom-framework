/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/restart_session.inc
 * PURPOSE: Restart through the active adapter while retiring old inspection references.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Some adapters emit initialized while processing restart and wait for the
 * client to configure them before acknowledging restart. Reuse canonical
 * breakpoint synchronization, with the original operation's timeout budget. */
static UmiStatus RestartConfigure(UmiDebugRuntimePlatform *platform)
{
    platform->restartConfigurationPending = 0;
    UmiDebugBreakpointRegistry *registry = umi_debug_service_breakpoint(platform->service);
    UmiStatus status = UMI_STATUS_OK;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < umi_debug_breakpoint_registry_count(registry);
         ++index)
    {
        UmiDebugBreakpointSnapshot current;
        status = umi_debug_breakpoint_registry_at(registry, index, &current);
        if (status != UMI_STATUS_OK)
            break;
        bool first = true;
        for (size_t previous = 0U; previous < index; ++previous)
        {
            UmiDebugBreakpointSnapshot other;
            status = umi_debug_breakpoint_registry_at(registry, previous, &other);
            if (status != UMI_STATUS_OK)
                break;
            if (strcmp(current.uri, other.uri) == 0)
            {
                first = false;
                break;
            }
        }
        if (status != UMI_STATUS_OK)
            break;
        if (first)
        {
            uint32_t remaining = DebugDeadlineRemaining(&platform->restartDeadline);
            if (remaining == 0U)
            {
                status = UMI_STATUS_TIMEOUT;
                break;
            }
            status = umi_debug_runtime_platform_sync_breakpoints(platform, current.uri, remaining);
        }
    }
    if (status == UMI_STATUS_OK && platform->capabilities.supports_configuration_done)
    {
        UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
        if (response == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            uint32_t remaining = DebugDeadlineRemaining(&platform->restartDeadline);
            uint64_t sequence = 0U;
            if (remaining == 0U)
                status = UMI_STATUS_TIMEOUT;
            else
                status = umi_debug_runtime_request_configuration_done(platform->adapter, &sequence);
            if (status == UMI_STATUS_OK)
                status =
                    umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, remaining, response);
            if (status == UMI_STATUS_OK && strcmp(response->command, "configurationDone") != 0)
                status = UMI_STATUS_PARSE_ERROR;
            free(response);
        }
    }
    if (status != UMI_STATUS_OK)
        platform->restartUncertain = 1;
    return status;
}
static UmiStatus RestartObserve(void *context, const UmiDebugRuntimeEnvelope *event,
                                uint32_t remainingMilliseconds, int *consumed)
{
    UmiDebugRuntimePlatform *platform = context;
    *consumed = 0;
    if (strcmp(event->event, "initialized") != 0)
        return UMI_STATUS_OK;
    if (remainingMilliseconds == 0U)
        return UMI_STATUS_TIMEOUT;
    *consumed = 1;
    return publish_event_envelope(platform, event);
}

UmiStatus UmiDebugRuntimePlatformCheckRestart(UmiDebugRuntimePlatform *platform)
{
    if (platform == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || !platform->initialized || platform->adapter == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_restart)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (platform->restartUncertain)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    UmiStatus status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK)
        return status;
    if (adapter.queued_events != 0U || adapter.queued_responses != 0U || adapter.pending_requests != 0U ||
        platform->restartConfigurationPending)
        return UMI_STATUS_BUSY;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED &&
        adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    if (umi_debug_watch_registry_revision(umi_debug_service_watch(platform->service)) == UINT64_MAX ||
        platform->revision == UINT64_MAX ||
        umi_debug_thread_registry_revision(umi_debug_service_thread(platform->service)) == UINT64_MAX ||
        umi_debug_stack_frame_registry_revision(umi_debug_service_stack_frame(platform->service)) ==
            UINT64_MAX ||
        umi_debug_scope_registry_revision(umi_debug_service_scope(platform->service)) == UINT64_MAX ||
        umi_debug_variable_registry_revision(umi_debug_service_variable(platform->service)) == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
UmiStatus umi_debug_runtime_platform_restart(UmiDebugRuntimePlatform *platform, uint32_t timeout_ms)
{
    if (timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugRuntimePlatformCheckRestart(platform);
    if (status != UMI_STATUS_OK)
        return status;
    /* Allocate the large envelope before changing state. Failed allocation
     * leaves the currently displayed stopped inspection available. */
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    if (response == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiDebugWatchRegistryInvalidateValues(umi_debug_service_watch(platform->service));
    if (status != UMI_STATUS_OK)
    {
        free(response);
        return status;
    }
    umi_debug_thread_registry_clear(umi_debug_service_thread(platform->service));
    umi_debug_stack_frame_registry_clear(umi_debug_service_stack_frame(platform->service));
    umi_debug_scope_registry_clear(umi_debug_service_scope(platform->service));
    umi_debug_variable_registry_clear(umi_debug_service_variable(platform->service));
    platform->paused = 0;
    platform->active_thread_id = 0U;
    platform->active_frame_id = 0U;
    ++platform->revision;
    platform->restartUncertain = 1;
    platform->restartConfigurationPending = 1;
    platform->restartDeadline = DebugDeadlineStart(timeout_ms);
    (void)umi_debug_runtime_adapter_set_state(platform->adapter, UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
    (void)umi_debug_inspection_session_set_state(umi_debug_advanced_platform_inspection(platform->advanced),
                                                 UMI_DEBUG_INSPECTION_RUNNING);
    /* A transport failure can occur after bytes reached the adapter. Retain
     * uncertainty rather than restoring references or repeating the mutation. */
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_restart(platform->adapter, NULL, &sequence);
    if (status == UMI_STATUS_OK)
        status = UmiDebugRuntimeAdapterWaitObserved(platform->adapter, sequence,
                                                    DebugDeadlineRemaining(&platform->restartDeadline),
                                                    RestartObserve, platform, response);
    if (status == UMI_STATUS_OK && strcmp(response->command, "restart") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    free(response);
    if (status != UMI_STATUS_OK)
        return status;
    status = umi_debug_runtime_publish_session(
        &platform->bridge, platform->active_configuration_id, platform->descriptor.id, "running",
        UMI_DEBUG_INSPECTION_RUNNING, platform->attached, platform->capabilities.supports_restart);
    if (status == UMI_STATUS_OK)
        platform->restartUncertain = 0;
    /* Interleaved events remain queued. Normal polling publishes the new stop
     * and obtains fresh frame and variable references before inspection. */
    return status;
}
