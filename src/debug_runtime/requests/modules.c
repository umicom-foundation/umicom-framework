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

/*
 * Provide the debug runtime request modules operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_modules(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t start_module,
    uint32_t module_count,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"startModule\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, start_module);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"moduleCount\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, module_count);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "modules", arguments, "", out_sequence);
}
