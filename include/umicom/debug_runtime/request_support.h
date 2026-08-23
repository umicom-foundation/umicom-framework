/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/request_support.h
 *
 * PURPOSE:
 *   Share bounded DAP argument builders across method-specific request modules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SUPPORT_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SUPPORT_H
#include "umicom/debug_runtime/adapter.h"
#include "umicom/language_runtime/json_writer.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_debug_runtime_request_no_arguments(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t *out_sequence);

UmiStatus umi_debug_runtime_request_thread(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t thread_id,
    uint64_t *out_sequence);

UmiStatus umi_debug_runtime_request_frame(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t frame_id,
    uint64_t *out_sequence);

UmiStatus umi_debug_runtime_request_reference(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *key,
    uint64_t reference,
    uint64_t *out_sequence);

UmiStatus umi_debug_runtime_request_raw(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint64_t *out_sequence);

#ifdef __cplusplus
}
#endif
#endif
