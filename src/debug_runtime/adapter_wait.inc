/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter_wait.inc
 *
 * PURPOSE:
 *   Correlate debugger replies while retaining messages that cross a positive wait deadline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "adapter_wait_private.h"

UmiStatus umi_debug_runtime_adapter_wait_response(
    UmiDebugRuntimeAdapter *adapter, uint64_t request_sequence,
    uint32_t timeout_ms, UmiDebugRuntimeEnvelope *out_response)
{
    if (adapter == NULL || request_sequence == 0U || out_response == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    DebugDeadline deadline = DebugDeadlineStart(timeout_ms);
    /* One heap candidate keeps caller output atomic and avoids adding another
     * large protocol envelope to the stack during nested configuration waits. */
    UmiDebugRuntimeEnvelope *envelope = malloc(sizeof(*envelope));
    if (envelope == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_TIMEOUT;
    if (DebugDeadlineExpired(&deadline)) goto done;
    status = umi_debug_runtime_response_queue_take(adapter->responses, request_sequence, envelope);
    if (status == UMI_STATUS_OK) {
        if (DebugDeadlineExpired(&deadline)) {
            status = DebugWaitRetainLate(adapter, envelope); goto done;
        }
        *out_response = *envelope;
        status = envelope->success ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
        goto done;
    }
    if (status != UMI_STATUS_NOT_FOUND) goto done;
    for (unsigned attempt = 0U; attempt < 256U; ++attempt) {
        if (DebugDeadlineExpired(&deadline)) { status = UMI_STATUS_TIMEOUT; goto done; }
        status = umi_debug_runtime_adapter_receive(adapter, DebugDeadlineRemaining(&deadline), envelope);
        if (status == UMI_STATUS_NOT_FOUND) {
            if (DebugDeadlineRemaining(&deadline) == 0U) { status = UMI_STATUS_TIMEOUT; goto done; }
            continue;
        }
        if (status != UMI_STATUS_OK) goto done;
        if (DebugDeadlineExpired(&deadline)) {
            status = DebugWaitRetainLate(adapter, envelope); goto done;
        }
        if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
            status = umi_debug_runtime_event_queue_push(adapter->events, envelope);
            if (status != UMI_STATUS_OK) goto done;
        } else if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE) {
            if (envelope->request_sequence == request_sequence) {
                *out_response = *envelope;
                status = envelope->success ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
                goto done;
            }
            status = umi_debug_runtime_response_queue_push(adapter->responses, envelope);
            if (status != UMI_STATUS_OK) goto done;
        }
    }
    status = UMI_STATUS_TIMEOUT;
done:
    free(envelope);
    return status;
}
