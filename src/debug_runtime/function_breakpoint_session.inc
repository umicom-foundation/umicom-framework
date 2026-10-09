/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/function_breakpoint_session.inc
 * PURPOSE: Keep requested function breakpoints and acknowledgements with the native debugger owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The platform owns this state on the same thread as its adapter. A new session
 * resets choices instead of carrying another process's conditions into this one. */
static UmiStatus DebugFunctionInitialize(UmiDebugRuntimePlatform *platform)
{
    if (platform->functions.draft.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t revision = platform->functions.draft.revision + 1U;
    memset(&platform->functions, 0, sizeof platform->functions);
    platform->functions.draft.revision = revision;
    platform->functions.supported = platform->capabilities.supports_function_breakpoints;
    platform->functions.conditions_supported =
        platform->capabilities.supports_conditional_breakpoints;
    platform->functions.hit_conditions_supported = platform->capabilities.supports_hit_conditions;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeFunctionBreakpointsRead(const UmiDebugRuntimePlatform *platform,
                                                 UmiDebugFunctionSnapshot *out)
{
    if (platform == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out = platform->functions;
    snprintf(out->draft.session_id, sizeof out->draft.session_id, "%s",
             platform->active_session_id);
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeFunctionBreakpointsApply(UmiDebugRuntimePlatform *platform,
                                                  const UmiDebugFunctionDraft *draft,
                                                  uint32_t timeout_ms)
{
    if (platform == NULL || draft == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || platform->adapter == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (!platform->functions.supported)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiStatus status = UmiDebugFunctionDraftValidate(draft);
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(draft->session_id, platform->active_session_id) != 0 ||
        draft->revision != platform->functions.draft.revision)
        return UMI_STATUS_INVALID_STATE;
    if (platform->functions.draft.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t enabled = 0U;
    for (size_t i = 0U; i < draft->count; ++i)
    {
        const UmiDebugFunctionEntry *entry = &draft->entries[i];
        if (!entry->enabled)
            continue;
        if ((entry->breakpoint.condition[0] != '\0' && !platform->functions.conditions_supported) ||
            (entry->breakpoint.hit_condition[0] != '\0' &&
             !platform->functions.hit_conditions_supported))
            return UMI_STATUS_NOT_IMPLEMENTED;
        ++enabled;
    }
    UmiDebugRuntimeFunctionBreakpoint *requested =
        calloc(UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT, sizeof *requested);
    UmiDebugRuntimeEnvelope *reply = malloc(sizeof *reply);
    if (requested == NULL || reply == NULL)
    {
        free(requested);
        free(reply);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t position = 0U;
    for (size_t i = 0U; i < draft->count; ++i)
        if (draft->entries[i].enabled)
            requested[position++] = draft->entries[i].breakpoint;
    uint64_t revision = platform->functions.draft.revision + 1U;
    platform->functions.draft = *draft;
    platform->functions.draft.revision = revision;
    platform->functions.acknowledged = 0;
    memset(&platform->functions.reply, 0, sizeof platform->functions.reply);
    platform->functions.reply.count = enabled;
    for (size_t i = 0U; i < enabled; ++i)
        platform->functions.reply.entries[i].verified = -1;
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_set_function_breakpoints(platform->adapter, requested,
                                                                enabled, &sequence);
    if (status == UMI_STATUS_OK)
        status =
            umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, reply);
    if (status == UMI_STATUS_OK)
        status = UmiDebugFunctionReplyDecode(reply->json, enabled, &platform->functions.reply);
    platform->functions.acknowledged = status == UMI_STATUS_OK;
    free(reply);
    free(requested);
    return status;
}
