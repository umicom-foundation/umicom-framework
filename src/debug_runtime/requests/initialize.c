/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/initialize.c
 *
 * PURPOSE:
 *   Implement the DAP initialize request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/initialize.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Initialise debug runtime request from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_runtime_request_initialize(
    UmiDebugRuntimeAdapter *adapter,
    const char *adapter_id,
    uint64_t *out_sequence)
{
    char arguments[2048];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "initialize", arguments, "", out_sequence);
}
