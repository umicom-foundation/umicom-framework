/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter_observed_wait.inc
 * PURPOSE: Allow protocol initialization events to be serviced during a correlated debugger request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDebugRuntimeAdapterWaitObserved(UmiDebugRuntimeAdapter *adapter, uint64_t requestSequence,
                                             uint32_t timeoutMilliseconds,
                                             UmiDebugRuntimeEventObserver observer, void *context,
                                             UmiDebugRuntimeEnvelope *outResponse)
{
    if (adapter == NULL || requestSequence == 0U || outResponse == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugRuntimeEnvelope *envelope = malloc(sizeof *envelope);
    if (envelope == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    DebugDeadline deadline = DebugDeadlineStart(timeoutMilliseconds);
    UmiStatus status = UMI_STATUS_TIMEOUT;
    for (unsigned attempt = 0U; attempt < 256U; ++attempt)
    {
        /* A nested configuration request may already have received our reply.
         * Taking it first avoids waiting for a second response that cannot come. */
        status = umi_debug_runtime_response_queue_take(adapter->responses, requestSequence, envelope);
        if (status == UMI_STATUS_OK)
        {
            *outResponse = *envelope;
            status = envelope->success ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
            goto done;
        }
        if (status != UMI_STATUS_NOT_FOUND)
            goto done;
        uint32_t remaining = DebugDeadlineRemaining(&deadline);
        if (remaining == 0U)
        {
            status = UMI_STATUS_TIMEOUT;
            goto done;
        }
        status = umi_debug_runtime_adapter_receive(adapter, remaining, envelope);
        if (status == UMI_STATUS_NOT_FOUND)
            continue;
        if (status != UMI_STATUS_OK)
            goto done;
        if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT)
        {
            int consumed = 0;
            UmiStatus observed = UMI_STATUS_OK;
            if (observer != NULL)
                observed = observer(context, envelope, DebugDeadlineRemaining(&deadline), &consumed);
            /* A declined event remains owned by the ordinary event queue even
             * when its observer reports failure. Callers can inspect it while
             * deciding how to recover; it must not disappear silently. */
            if (!consumed)
            {
                status = umi_debug_runtime_event_queue_push(adapter->events, envelope);
                if (status != UMI_STATUS_OK)
                    goto done;
            }
            if (observed != UMI_STATUS_OK)
            {
                status = observed;
                goto done;
            }
        }
        else if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE)
        {
            if (envelope->request_sequence == requestSequence)
            {
                *outResponse = *envelope;
                status = envelope->success ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
                goto done;
            }
            status = umi_debug_runtime_response_queue_push(adapter->responses, envelope);
            if (status != UMI_STATUS_OK)
                goto done;
        }
    }
    status = UMI_STATUS_TIMEOUT;
done:
    free(envelope);
    return status;
}
