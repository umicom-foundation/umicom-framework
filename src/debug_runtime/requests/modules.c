/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/modules.c
 *
 * PURPOSE:
 *   Implement the DAP modules request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/modules.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_modules(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t start_module,
    uint32_t module_count,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"startModule\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, start_module);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"moduleCount\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, module_count);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "modules", arguments, "", out_sequence);
}
