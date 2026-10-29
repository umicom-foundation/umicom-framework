/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter_wait_private.h
 *
 * PURPOSE:
 *   Retain a decoded late debugger message without publishing timed-out caller output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_ADAPTER_WAIT_PRIVATE_H
#define UMICOM_DEBUG_RUNTIME_ADAPTER_WAIT_PRIVATE_H
#include "adapter_private.h"
#include "deadline.h"

/* Receive has already consumed this frame and resolved pending correlation.
 * Preserve its ownership in the appropriate bounded queue. Queue exhaustion
 * is reported explicitly instead of claiming the message was retained. */
static UmiStatus DebugWaitRetainLate(UmiDebugRuntimeAdapter *adapter,
    const UmiDebugRuntimeEnvelope *envelope)
{
    UmiStatus status = UMI_STATUS_OK;
    if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE)
        status = umi_debug_runtime_response_queue_push(adapter->responses, envelope);
    else if (envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT)
        status = umi_debug_runtime_event_queue_push(adapter->events, envelope);
    return status == UMI_STATUS_OK ? UMI_STATUS_TIMEOUT : status;
}
#endif
