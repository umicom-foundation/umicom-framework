/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/response_queue.h
 *
 * PURPOSE:
 *   Preserve out-of-order DAP responses in opaque heap-owned storage so large
 *   JSON messages do not inflate public structs or normal Windows thread stacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_RESPONSE_QUEUE_H
#define UMICOM_DEBUG_RUNTIME_RESPONSE_QUEUE_H
#include "umicom/debug_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime response queue data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeResponseQueue UmiDebugRuntimeResponseQueue;

/**
 * Initialise debug runtime response queue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_runtime_response_queue_create(
    UmiDebugRuntimeResponseQueue **out_queue);
/**
 * Release or reset state held by debug runtime response queue so the same storage can be
 * reused safely.
 */
void umi_debug_runtime_response_queue_destroy(
    UmiDebugRuntimeResponseQueue *queue);
/**
 * Provide the debug runtime response queue push operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_response_queue_push(
    UmiDebugRuntimeResponseQueue *queue,
    const UmiDebugRuntimeEnvelope *response);
/**
 * Provide the debug runtime response queue take operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_response_queue_take(
    UmiDebugRuntimeResponseQueue *queue,
    uint64_t request_sequence,
    UmiDebugRuntimeEnvelope *out_response);
/**
 * Return the number of records represented by debug runtime response queue without
 * changing their state.
 */
size_t umi_debug_runtime_response_queue_count(
    const UmiDebugRuntimeResponseQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
