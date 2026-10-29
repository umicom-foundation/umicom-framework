/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter_private.h
 *
 * PURPOSE:
 *   Share the owned adapter layout with its reply waits and deterministic regression seam.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_ADAPTER_PRIVATE_H
#define UMICOM_DEBUG_RUNTIME_ADAPTER_PRIVATE_H
#include "umicom/debug_runtime/adapter.h"

struct UmiDebugRuntimeAdapter {
    char adapter_id[UMI_DEBUG_RUNTIME_ID_CAPACITY];
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeFramer framer;
    UmiDebugRuntimePendingTable pending;
    UmiDebugRuntimeEventQueue *events;
    UmiDebugRuntimeResponseQueue *responses;
    UmiDebugRuntimeAdapterState state;
    uint64_t next_sequence;
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t revision;
};
#endif
