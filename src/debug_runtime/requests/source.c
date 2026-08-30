/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/source.c
 *
 * PURPOSE:
 *   Implement the DAP source request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/source.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_source(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint64_t source_reference,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL ||
        ((source_path == NULL || source_path[0] == '\0') &&
         source_reference == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"source\":{");
    if (source_path != NULL && source_path[0] != '\0') {
        (void)umi_language_runtime_json_writer_raw(&writer, "\"path\":");
        (void)umi_language_runtime_json_writer_string(&writer, source_path);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "},\"sourceReference\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, source_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "source", arguments,
        source_path != NULL ? source_path : "", out_sequence);
}
