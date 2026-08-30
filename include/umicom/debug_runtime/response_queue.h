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

typedef struct UmiDebugRuntimeResponseQueue UmiDebugRuntimeResponseQueue;

UmiStatus umi_debug_runtime_response_queue_create(
    UmiDebugRuntimeResponseQueue **out_queue);
void umi_debug_runtime_response_queue_destroy(
    UmiDebugRuntimeResponseQueue *queue);
UmiStatus umi_debug_runtime_response_queue_push(
    UmiDebugRuntimeResponseQueue *queue,
    const UmiDebugRuntimeEnvelope *response);
UmiStatus umi_debug_runtime_response_queue_take(
    UmiDebugRuntimeResponseQueue *queue,
    uint64_t request_sequence,
    UmiDebugRuntimeEnvelope *out_response);
size_t umi_debug_runtime_response_queue_count(
    const UmiDebugRuntimeResponseQueue *queue);

#ifdef __cplusplus
}
#endif
#endif
