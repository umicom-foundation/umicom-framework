/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/memory_inspection/peer.inc
 * PURPOSE: Provide deterministic memory protocol responses without accessing any debuggee.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json.h"
static int MemoryPeerReply(const UmiDebugRuntimeEnvelope *request, const char *mode, const char *json)
{
    if (strcmp(request->command, "initialize") == 0)
        return Response(
            request->sequence, request->command, 1,
            strcmp(mode, "memory-inspection-unsupported") == 0
                ? "{\"supportsConfigurationDoneRequest\":true}"
                : "{\"supportsConfigurationDoneRequest\":true,\"supportsReadMemoryRequest\":true}");
    if (strcmp(request->command, "variables") == 0)
        return Response(
            request->sequence, request->command, 1,
            strcmp(mode, "memory-inspection-no-reference") == 0
                ? "{\"variables\":[{\"name\":\"bytes\",\"value\":\"buffer\",\"variablesReference\":0}]}"
                : "{\"variables\":[{\"name\":\"bytes\",\"value\":\"buffer\",\"variablesReference\":0,"
                  "\"memoryReference\":\"opaque:buffer\"}]}");
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    if (doc == NULL)
        return 0;
    int valid = umi_language_runtime_json_parse(json, doc) == UMI_STATUS_OK;
    int args = valid ? umi_language_runtime_json_object_get(doc, 0, "arguments") : -1;
    char reference[128] = {0};
    int64_t offset = 0, count = 0;
    valid = valid &&
            umi_language_runtime_json_string(
                doc, umi_language_runtime_json_object_get(doc, args, "memoryReference"), reference,
                sizeof reference) == UMI_STATUS_OK &&
            umi_language_runtime_json_int64(doc, umi_language_runtime_json_object_get(doc, args, "offset"),
                                            &offset) == UMI_STATUS_OK &&
            umi_language_runtime_json_int64(doc, umi_language_runtime_json_object_get(doc, args, "count"),
                                            &count) == UMI_STATUS_OK;
    free(doc);
    if (!valid || strcmp(reference, "opaque:buffer") != 0 || (offset != 0 && offset != -16) || count != 256)
        return 0;
    if (strcmp(mode, "memory-inspection-offset") == 0 && offset != -16)
        return 0;
    if (strcmp(mode, "memory-inspection-rejected") == 0)
        return Response(request->sequence, "readMemory", 0, "{}");
    if (strcmp(mode, "memory-inspection-timeout") == 0)
        return 1;
    if (strcmp(mode, "memory-inspection-event") == 0 && !Event("continued", "{\"threadId\":1}"))
        return 0;
    static unsigned reads = 0U;
    ++reads;
    if (strcmp(mode, "memory-inspection-malformed") == 0 ||
        (strcmp(mode, "memory-inspection-failed-refresh") == 0 && reads > 1U))
        return Response(request->sequence, "readMemory", 1, "{\"address\":\"0x1000\",\"data\":\"AA==AAAA\"}");
    if (strcmp(mode, "memory-inspection-empty") == 0)
        return Response(request->sequence, "readMemory", 1, "{\"address\":\"0x1000\",\"unreadableBytes\":8}");
    if (strcmp(mode, "memory-inspection-unreadable") == 0)
        return Response(request->sequence, "readMemory", 1,
                        "{\"address\":\"0x1000\",\"data\":\"QUJDAH+A\",\"unreadableBytes\":20}");
    return Response(request->sequence,
                    strcmp(mode, "memory-inspection-command") == 0 ? "evaluate" : "readMemory", 1,
                    "{\"address\":\"0x1000\",\"data\":\"QUJDAH+A\"}");
}
