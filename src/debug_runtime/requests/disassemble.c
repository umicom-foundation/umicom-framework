/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/disassemble.c
 *
 * PURPOSE:
 *   Implement the DAP disassemble request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/disassemble.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_disassemble(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    int64_t instruction_offset,
    uint32_t instruction_count,
    int resolve_symbols,
    uint64_t *out_sequence)
{
    char arguments[1024];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || memory_reference == NULL ||
        memory_reference[0] == '\0' || instruction_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"memoryReference\":");
    (void)umi_language_runtime_json_writer_string(&writer, memory_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"offset\":");
    (void)umi_language_runtime_json_writer_int64(&writer, offset);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"instructionOffset\":");
    (void)umi_language_runtime_json_writer_int64(
        &writer, instruction_offset);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"instructionCount\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, instruction_count);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"resolveSymbols\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, resolve_symbols);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "disassemble", arguments, memory_reference, out_sequence);
}
