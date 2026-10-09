/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/transport.h
 *
 * PURPOSE:
 *   Reuse the persistent byte-stream transport implemented by the Language
 *   Runtime for DAP without duplicating process or pipe code. DAP and LSP both
 *   use framed JSON over a persistent byte stream.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Check that debug runtime transport satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_debug_runtime_transport_validate(
    const UmiDebugRuntimeTransport *transport);

/**
 * Start the same owned DAP process with one project-selected tool folder.
 * NULL/empty tool_directory retains inherited selection. A simple tool name
 * resolves only inside an explicit absolute folder; an absolute executable is
 * kept. Only the child receives the PATH prefix. No trust is granted here.
 * Arguments and folder strings are borrowed until return. Failure clears the
 * output; successful ownership follows the ordinary start-process contract.
 */
UmiStatus UmiDebugRuntimeTransportStartProcessWithToolDirectory(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    const char *tool_directory,
    UmiDebugRuntimeTransport *out_transport);

/**
 * Provide the debug runtime transport start process operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_transport_start_process(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiDebugRuntimeTransport *out_transport);

/**
 * Initialise debug runtime memory transport from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_runtime_memory_transport_create(
    UmiDebugRuntimeMemoryTransport **out_memory,
    UmiDebugRuntimeTransport *out_transport);

/**
 * Read debug runtime memory transport push into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_debug_runtime_memory_transport_push_read(
    UmiDebugRuntimeMemoryTransport *memory,
    const void *bytes,
    size_t byte_count);

/**
 * Provide the debug runtime memory transport written operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_memory_transport_written(
    const UmiDebugRuntimeMemoryTransport *memory,
    char *out_text,
    size_t capacity,
    size_t *out_count);

/** Start the DAP transport with reviewed child environment overrides.
 * definitions follows UmiProcessEnvironmentValidate; NULL keeps inherited values.
 * The tools directory, process lifetime and output ownership match the existing
 * WithToolDirectory entry point. No host environment is changed. */
UmiStatus UmiDebugRuntimeTransportStartProcessWithEnvironment(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    const char *tool_directory,
    const char *definitions,
    UmiDebugRuntimeTransport *out_transport);

#ifdef __cplusplus
}
#endif
#endif
