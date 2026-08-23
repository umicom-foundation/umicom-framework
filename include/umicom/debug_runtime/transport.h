/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/transport.h
 *
 * PURPOSE:
 *   Reuse the persistent byte-stream transport implemented by the Language
 *   Runtime for DAP without duplicating process or pipe code. DAP and LSP both
 *   use framed JSON over a persistent byte stream.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_TRANSPORT_H
#define UMICOM_DEBUG_RUNTIME_TRANSPORT_H

#include "umicom/language_runtime/transport.h"
#include "umicom/language_runtime/memory_transport.h"
#include "umicom/language_runtime/framing.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiLanguageRuntimeTransport UmiDebugRuntimeTransport;
typedef UmiLanguageRuntimeMemoryTransport UmiDebugRuntimeMemoryTransport;
typedef UmiLanguageRuntimeFramer UmiDebugRuntimeFramer;

UmiStatus umi_debug_runtime_transport_validate(
    const UmiDebugRuntimeTransport *transport);

UmiStatus umi_debug_runtime_transport_start_process(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiDebugRuntimeTransport *out_transport);

UmiStatus umi_debug_runtime_memory_transport_create(
    UmiDebugRuntimeMemoryTransport **out_memory,
    UmiDebugRuntimeTransport *out_transport);

UmiStatus umi_debug_runtime_memory_transport_push_read(
    UmiDebugRuntimeMemoryTransport *memory,
    const void *bytes,
    size_t byte_count);

UmiStatus umi_debug_runtime_memory_transport_written(
    const UmiDebugRuntimeMemoryTransport *memory,
    char *out_text,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
