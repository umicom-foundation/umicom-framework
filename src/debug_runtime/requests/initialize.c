/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/initialize.c
 *
 * PURPOSE:
 *   Implement the DAP initialize request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/initialize.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_initialize(
    UmiDebugRuntimeAdapter *adapter,
    const char *adapter_id,
    uint64_t *out_sequence)
{
    char arguments[2048];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || adapter_id == NULL || adapter_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer,
        "{\"clientID\":\"umicom-framework\",\"clientName\":\"Umicom Framework\","
        "\"adapterID\":");
    (void)umi_language_runtime_json_writer_string(&writer, adapter_id);
    (void)umi_language_runtime_json_writer_raw(
        &writer,
        ",\"locale\":\"en-GB\",\"linesStartAt1\":true,\"columnsStartAt1\":true,"
        "\"pathFormat\":\"path\",\"supportsVariableType\":true,"
        "\"supportsVariablePaging\":true,\"supportsRunInTerminalRequest\":true,"
        "\"supportsMemoryReferences\":true,\"supportsProgressReporting\":true,"
        "\"supportsInvalidatedEvent\":true,\"supportsMemoryEvent\":true}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "initialize", arguments, "", out_sequence);
}
