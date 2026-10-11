/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/thread_control.inc
 * PURPOSE: Retire stopped-frame evidence before a debugger control request can resume execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Keep command identity beside the request callback. A response for another
 * operation must not turn a timed-out or misrouted request into success. */
static const char *ThreadControlCommand(UmiStatus (*request)(UmiDebugRuntimeAdapter *, uint64_t,
                                                             uint64_t *))
{
    if (request == umi_debug_runtime_request_continue)
        return "continue";
    if (request == umi_debug_runtime_request_pause)
        return "pause";
    if (request == umi_debug_runtime_request_next)
        return "next";
    if (request == umi_debug_runtime_request_step_in)
        return "stepIn";
    if (request == umi_debug_runtime_request_step_out)
        return "stepOut";
    if (request == umi_debug_runtime_request_step_back)
        return "stepBack";
    if (request == umi_debug_runtime_request_reverse_continue)
        return "reverseContinue";
    return NULL;
}

/* Resume operations invalidate adapter object references even if their reply
 * never reaches the client. Preflight every generation before clearing any
 * collection so exhaustion refuses without partially retiring the current view. */
static UmiStatus ThreadControlCanRetire(UmiDebugRuntimePlatform *platform)
{
    if (platform->revision == UINT64_MAX ||
        umi_debug_watch_registry_revision(umi_debug_service_watch(platform->service)) ==
            UINT64_MAX ||
        umi_debug_thread_registry_revision(umi_debug_service_thread(platform->service)) ==
            UINT64_MAX ||
        umi_debug_stack_frame_registry_revision(umi_debug_service_stack_frame(platform->service)) ==
            UINT64_MAX ||
        umi_debug_scope_registry_revision(umi_debug_service_scope(platform->service)) ==
            UINT64_MAX ||
        umi_debug_variable_registry_revision(umi_debug_service_variable(platform->service)) ==
            UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* The native connection owns execution state. Applications continue using the
 * established control functions, while this shared boundary prevents stale
 * variables and instruction captures from surviving an attempted resume. */
static UmiStatus
thread_control(UmiDebugRuntimePlatform *platform, uint64_t thread_id, uint32_t timeout_ms,
               UmiStatus (*request)(UmiDebugRuntimeAdapter *, uint64_t, uint64_t *),
               UmiDebugRuntimeAdapterState state_after_response)
{
    const char *command = ThreadControlCommand(request);
    if (platform == NULL || platform->adapter == NULL || command == NULL || thread_id == 0U ||
        thread_id > INT32_MAX || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool resume = state_after_response == UMI_DEBUG_RUNTIME_ADAPTER_RUNNING;
    if (state_after_response != UMI_DEBUG_RUNTIME_ADAPTER_RUNNING &&
        state_after_response != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!platform->active || !platform->initialized || platform->restartUncertain)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    UmiStatus status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK)
        return status;
    if (adapter.queued_events != 0U || adapter.queued_responses != 0U ||
        adapter.pending_requests != 0U || platform->restartConfigurationPending)
        return UMI_STATUS_BUSY;
    if ((resume && (!platform->paused || adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)) ||
        (!resume && (platform->paused || adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_RUNNING)))
        return UMI_STATUS_INVALID_STATE;
    status = ThreadControlCanRetire(platform);
    if (status != UMI_STATUS_OK)
        return status;

    struct ControlExchange
    {
        UmiDebugRuntimeEnvelope response;
        UmiLanguageRuntimeJsonDocument document;
    };
    struct ControlExchange *exchange = malloc(sizeof *exchange);
    if (exchange == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (resume)
    {
        status = UmiDebugWatchRegistryInvalidateValues(umi_debug_service_watch(platform->service));
        if (status != UMI_STATUS_OK)
        {
            free(exchange);
            return status;
        }
        umi_debug_thread_registry_clear(umi_debug_service_thread(platform->service));
        umi_debug_stack_frame_registry_clear(umi_debug_service_stack_frame(platform->service));
        umi_debug_scope_registry_clear(umi_debug_service_scope(platform->service));
        umi_debug_variable_registry_clear(umi_debug_service_variable(platform->service));
        platform->paused = 0;
        platform->stopped_by_exception = 0;
        platform->active_frame_id = 0U;
    }
    ++platform->revision;
    /* Do not restore old frames on failure: bytes may already have reached the
     * adapter. A new stopped event permits fresh inspection. Otherwise the user
     * can end the session and reconnect; this owner never retries a resume. */
    uint64_t sequence = 0U;
    status = request(platform->adapter, thread_id, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         &exchange->response);
    int body = -1;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_json_parse(exchange->response.json, &exchange->document);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(&exchange->document, command,
                                           strcmp(command, "continue") != 0, &body);
    if (status == UMI_STATUS_OK && strcmp(command, "continue") == 0)
    {
        /* Some adapters resume only part of a process. The client conservatively
         * retires all prior captures and waits for fresh thread observations. */
        int all_threads = 0;
        status = DebugResponseJsonBoolean(&exchange->document, body, "allThreadsContinued", 0,
                                          &all_threads);
    }
    if (status == UMI_STATUS_OK)
    {
        platform->active_thread_id = thread_id;
        if (resume)
        {
            (void)umi_debug_runtime_adapter_set_state(platform->adapter,
                                                      UMI_DEBUG_RUNTIME_ADAPTER_RUNNING);
            (void)umi_debug_inspection_session_set_state(
                umi_debug_advanced_platform_inspection(platform->advanced),
                UMI_DEBUG_INSPECTION_RUNNING);
        }
        /* A pause acknowledgement is not proof of a stop. Only the existing
         * stopped-event publication makes paused inspection available again. */
    }
    free(exchange);
    return status;
}
