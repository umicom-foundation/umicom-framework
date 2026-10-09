/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/transport.c
 *
 * PURPOSE:
 *   Adapt the existing persistent Framework byte-stream implementation for DAP.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/transport.h"
#include "umicom/language_runtime/process_stream.h"

#include <string.h>

/*
 * Check that debug runtime transport satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_debug_runtime_transport_validate(
    const UmiDebugRuntimeTransport *transport)
{
    return umi_language_runtime_transport_validate(transport);
}

/*
 * Provide the debug runtime transport start process operation used by this module and its
 * client applications.
 */
/* Debugger child creation accepts the project tool directory through Framework-owned process transport. The compatibility entry point delegates to the same ownership and failure handling. The previous implementation is retained for engineering review. */
#if 0
UmiStatus umi_debug_runtime_transport_start_process(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiDebugRuntimeTransport *out_transport)
{
    UmiLanguageRuntimeProcessStreamConfig config;
    UmiLanguageRuntimeProcessStream *stream = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || out_transport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&config, 0, sizeof(config));
    config.program = program;
    config.arguments = arguments;
    config.argument_count = argument_count;
    config.working_directory = working_directory;
    config.merge_stderr = 0;

    status = umi_language_runtime_process_stream_start(&config, &stream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_transport_from_process(
        stream, out_transport);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_language_runtime_process_stream_destroy(stream);
    }

    return status;
}
#endif
/* Pass copied launch environment settings through the existing DAP process owner. The PATH-only public entry point remains as a wrapper; its preceding implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiDebugRuntimeTransportStartProcessWithToolDirectory(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    const char *tool_directory,
    UmiDebugRuntimeTransport *out_transport)
{
    UmiLanguageRuntimeProcessStreamConfig config;
    UmiLanguageRuntimeProcessStream *stream = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_transport != NULL) memset(out_transport, 0, sizeof(*out_transport));
    if (program == NULL || out_transport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&config, 0, sizeof(config));
    config.program = program;
    config.arguments = arguments;
    config.argument_count = argument_count;
    config.working_directory = working_directory;
    config.merge_stderr = 0;

    status = UmiLanguageRuntimeProcessStreamStartWithToolDirectory(&config, tool_directory, &stream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_transport_from_process(
        stream, out_transport);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_language_runtime_process_stream_destroy(stream);
    }

    return status;
}
#endif
UmiStatus UmiDebugRuntimeTransportStartProcessWithEnvironment(
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    const char *tool_directory,
    const char *definitions,
    UmiDebugRuntimeTransport *out_transport)
{
    UmiLanguageRuntimeProcessStreamConfig config;
    UmiLanguageRuntimeProcessStream *stream = NULL;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_transport != NULL) memset(out_transport, 0, sizeof(*out_transport));
    if (program == NULL || out_transport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&config, 0, sizeof(config));
    config.program = program;
    config.arguments = arguments;
    config.argument_count = argument_count;
    config.working_directory = working_directory;
    config.merge_stderr = 0;

    status = UmiLanguageRuntimeProcessStreamStartWithEnvironment(&config, tool_directory, definitions, &stream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_transport_from_process(
        stream, out_transport);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_language_runtime_process_stream_destroy(stream);
    }

    return status;
}

UmiStatus UmiDebugRuntimeTransportStartProcessWithToolDirectory(
    const char *program, const char *const *arguments, size_t argument_count,
    const char *working_directory, const char *tool_directory,
    UmiDebugRuntimeTransport *out_transport)
{
    return UmiDebugRuntimeTransportStartProcessWithEnvironment(program, arguments,
        argument_count, working_directory, tool_directory, NULL, out_transport);
}

/* Existing callers keep inherited tool selection. */
UmiStatus umi_debug_runtime_transport_start_process(
    const char *program, const char *const *arguments, size_t argument_count,
    const char *working_directory, UmiDebugRuntimeTransport *out_transport)
{
    return UmiDebugRuntimeTransportStartProcessWithToolDirectory(program, arguments,
        argument_count, working_directory, NULL, out_transport);
}

/*
 * Initialise debug runtime memory transport from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_runtime_memory_transport_create(
    UmiDebugRuntimeMemoryTransport **out_memory,
    UmiDebugRuntimeTransport *out_transport)
{
    return umi_language_runtime_memory_transport_create(
        out_memory, out_transport);
}

/*
 * Read debug runtime memory transport push into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_debug_runtime_memory_transport_push_read(
    UmiDebugRuntimeMemoryTransport *memory,
    const void *bytes,
    size_t byte_count)
{
    return umi_language_runtime_memory_transport_push_read(
        memory, bytes, byte_count);
}

/*
 * Provide the debug runtime memory transport written operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_memory_transport_written(
    const UmiDebugRuntimeMemoryTransport *memory,
    char *out_text,
    size_t capacity,
    size_t *out_count)
{
    return umi_language_runtime_memory_transport_written(
        memory, out_text, capacity, out_count);
}
