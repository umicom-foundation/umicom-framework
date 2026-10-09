/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/connection_identity.inc
 * PURPOSE: Keep delayed inspection requests tied to one native debugger connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Connection inspection now waits for pending lifecycle events and uncertain restarts to settle. The earlier identity-only reader is retained for review; copied data must not be labelled current while retirement events are pending. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeConnectionIdentityRead(const UmiDebugRuntimePlatform *platform,
                                                UmiDebugConnectionIdentity *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiDebugConnectionIdentity identity = {0};
    snprintf(identity.session_id, sizeof identity.session_id, "%s", platform->active_session_id);
    identity.generation = platform->inspection_generation;
    *out = identity;
    return UMI_STATUS_OK;
}
#endif
/* A published exit can leave the adapter process connected while its debuggee is gone. Inspection identity now requires a running or paused adapter state. The earlier event-queue-only gate is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeConnectionIdentityRead(const UmiDebugRuntimePlatform *platform,
                                                UmiDebugConnectionIdentity *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Pending events may retire the process or its source references. The host
     * must publish those events before presenting another current inspection. */
    if(!platform->initialized||platform->restartUncertain||platform->restartConfigurationPending)
        return UMI_STATUS_BUSY;
    UmiDebugRuntimeAdapterSnapshot snapshot;
    UmiStatus status=umi_debug_runtime_adapter_snapshot(platform->adapter,&snapshot);
    if(status!=UMI_STATUS_OK)return status;
    if(snapshot.queued_events!=0U)return UMI_STATUS_BUSY;
    UmiDebugConnectionIdentity identity = {0};
    snprintf(identity.session_id, sizeof identity.session_id, "%s", platform->active_session_id);
    identity.generation = platform->inspection_generation;
    *out = identity;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDebugRuntimeConnectionIdentityRead(const UmiDebugRuntimePlatform *platform,
                                                UmiDebugConnectionIdentity *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Pending events may retire the process or its source references. The host
     * must publish those events before presenting another current inspection. */
    if(!platform->initialized||platform->restartUncertain||platform->restartConfigurationPending)
        return UMI_STATUS_BUSY;
    UmiDebugRuntimeAdapterSnapshot snapshot;
    UmiStatus status=umi_debug_runtime_adapter_snapshot(platform->adapter,&snapshot);
    if(status!=UMI_STATUS_OK)return status;
    if(snapshot.queued_events!=0U)return UMI_STATUS_BUSY;
    if(snapshot.state==UMI_DEBUG_RUNTIME_ADAPTER_STOPPED)return UMI_STATUS_NOT_FOUND;
    if(snapshot.state!=UMI_DEBUG_RUNTIME_ADAPTER_RUNNING&&snapshot.state!=UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)
        return UMI_STATUS_BUSY;
    UmiDebugConnectionIdentity identity = {0};
    snprintf(identity.session_id, sizeof identity.session_id, "%s", platform->active_session_id);
    identity.generation = platform->inspection_generation;
    *out = identity;
    return UMI_STATUS_OK;
}
static UmiStatus DebugConnectionIdentityCheck(const UmiDebugRuntimePlatform *platform,
                                              const UmiDebugConnectionIdentity *expected)
{
    if (expected == NULL || memchr(expected->session_id, '\0', sizeof expected->session_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugConnectionIdentity actual;
    UmiStatus status = UmiDebugRuntimeConnectionIdentityRead(platform, &actual);
    if (status != UMI_STATUS_OK)
        return status;
    return actual.generation == expected->generation &&
                   strcmp(actual.session_id, expected->session_id) == 0
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_STATE;
}
