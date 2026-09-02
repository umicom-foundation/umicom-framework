/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/adapter.h
 *
 * PURPOSE:
 *   Own one persistent Debug Adapter Protocol connection, sequence numbers,
 *   response correlation and asynchronous event buffering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_ADAPTER_H
#define UMICOM_DEBUG_RUNTIME_ADAPTER_H

#include "umicom/debug_runtime/event_queue.h"
#include "umicom/debug_runtime/message.h"
#include "umicom/debug_runtime/pending.h"
#include "umicom/debug_runtime/response_queue.h"
#include "umicom/debug_runtime/transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime adapter data shared with callers of this public contract.
 */
typedef struct UmiDebugRuntimeAdapter UmiDebugRuntimeAdapter;

/**
 * Represent the debug runtime adapter snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeAdapterSnapshot {
    char adapter_id[UMI_DEBUG_RUNTIME_ID_CAPACITY];
    UmiDebugRuntimeAdapterState state;
    uint64_t next_sequence;
    size_t pending_requests;
    size_t queued_events;
    size_t queued_responses;
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t revision;
} UmiDebugRuntimeAdapterSnapshot;

/**
 * Provide the debug runtime adapter create with transport operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_runtime_adapter_create_with_transport(
    const char *adapter_id,
    UmiDebugRuntimeTransport *transport,
    UmiDebugRuntimeAdapter **out_adapter);

/**
 * Provide the debug runtime adapter start process operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_adapter_start_process(
    const char *adapter_id,
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiDebugRuntimeAdapter **out_adapter);

/**
 * Release or reset state held by debug runtime adapter so the same storage can be reused
 * safely.
 */
void umi_debug_runtime_adapter_destroy(UmiDebugRuntimeAdapter *adapter);

/**
 * Provide the debug runtime adapter send request operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_adapter_send_request(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint64_t *out_sequence);

/**
 * Provide the debug runtime adapter receive operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_adapter_receive(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_envelope);

/**
 * Provide the debug runtime adapter wait response operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_adapter_wait_response(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_sequence,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_response);

/**
 * Provide the debug runtime adapter invoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_adapter_invoke(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_response);

/**
 * Provide the debug runtime adapter next event operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_adapter_next_event(
    UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeEnvelope *out_event);

/**
 * Provide the debug runtime adapter set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_adapter_set_state(
    UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeAdapterState state);

/**
 * Provide the debug runtime adapter stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_adapter_stop(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t timeout_ms);

/**
 * Provide the debug runtime adapter is running operation used by this module and its
 * client applications.
 */
int umi_debug_runtime_adapter_is_running(
    UmiDebugRuntimeAdapter *adapter);

/**
 * Provide the debug runtime adapter snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_adapter_snapshot(
    const UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeAdapterSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
