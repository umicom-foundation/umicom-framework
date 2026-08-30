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

UmiStatus umi_debug_runtime_transport_validate(
    const UmiDebugRuntimeTransport *transport)
{
    return umi_language_runtime_transport_validate(transport);
}

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
    if (status != UMI_STATUS_OK) return status;

    status = umi_language_runtime_transport_from_process(
        stream, out_transport);
    if (status != UMI_STATUS_OK) {
        umi_language_runtime_process_stream_destroy(stream);
    }

    return status;
}

UmiStatus umi_debug_runtime_memory_transport_create(
    UmiDebugRuntimeMemoryTransport **out_memory,
    UmiDebugRuntimeTransport *out_transport)
{
    return umi_language_runtime_memory_transport_create(
        out_memory, out_transport);
}

UmiStatus umi_debug_runtime_memory_transport_push_read(
    UmiDebugRuntimeMemoryTransport *memory,
    const void *bytes,
    size_t byte_count)
{
    return umi_language_runtime_memory_transport_push_read(
        memory, bytes, byte_count);
}

UmiStatus umi_debug_runtime_memory_transport_written(
    const UmiDebugRuntimeMemoryTransport *memory,
    char *out_text,
    size_t capacity,
    size_t *out_count)
{
    return umi_language_runtime_memory_transport_written(
        memory, out_text, capacity, out_count);
}
