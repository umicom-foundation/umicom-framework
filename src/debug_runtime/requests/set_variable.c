/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_variable.c
 *
 * PURPOSE:
 *   Implement the DAP setVariable request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_variable.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_set_variable(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t variables_reference,
    const char *name,
    const char *value,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || variables_reference == 0U ||
        name == NULL || name[0] == '\0' || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"variablesReference\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, variables_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"name\":");
    (void)umi_language_runtime_json_writer_string(&writer, name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"value\":");
    (void)umi_language_runtime_json_writer_string(&writer, value);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setVariable", arguments, name, out_sequence);
}
