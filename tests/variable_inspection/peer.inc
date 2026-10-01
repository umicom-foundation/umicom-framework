/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_inspection/peer.inc
 * PURPOSE: Supply compound root and child responses through the existing deterministic DAP peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json.h"

static int VariablePeerReply(const UmiDebugRuntimeEnvelope *request, const char *mode, const char *json)
{
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    if (doc == NULL) return 0;
    int64_t reference = -1;
    UmiStatus status = umi_language_runtime_json_parse(json, doc);
    if (status == UMI_STATUS_OK) {
        int arguments = umi_language_runtime_json_object_get(doc, 0, "arguments");
        int token = umi_language_runtime_json_object_get(doc, arguments, "variablesReference");
        status = umi_language_runtime_json_int64(doc, token, &reference);
    }
    free(doc); if (status != UMI_STATUS_OK) return 0;
    if (reference == 1) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"notes\",\"value\":\"{...}\",\"type\":\"Notebook\",\"variablesReference\":2}]}");
    if (strcmp(mode, "variable-inspection-budget") == 0 && (reference == 2 || reference == 3)) {
        char body[8192]; strcpy(body, "{\"variables\":[");
        for (size_t i = 0U; i < 128U; ++i) strcat(body, i == 0U ?
            "{\"name\":\"i\",\"value\":\"v\",\"variablesReference\":3}" : ",{\"name\":\"i\",\"value\":\"v\",\"variablesReference\":3}");
        strcat(body, "]}"); return Response(request->sequence, "variables", 1, body);
    }
    if (reference == 3) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"leaf\",\"value\":\"7\",\"variablesReference\":0}]}");
    if (reference != 2) return 0;
    static unsigned childRequests = 0U; ++childRequests;
    if (strcmp(mode, "variable-inspection-failed-refresh") == 0 && childRequests > 1U)
        return Response(request->sequence, "variables", 1, "{\"variables\":[{\"name\":\"broken\"}]}");
    if (strcmp(mode, "variable-inspection-rejected") == 0) return Response(request->sequence, "variables", 0, "{}");
    if (strcmp(mode, "variable-inspection-timeout") == 0) return 1; /* Remain available for disconnect. */
    if (strcmp(mode, "variable-inspection-empty") == 0) return Response(request->sequence, "variables", 1, "{\"variables\":[]}");
    if (strcmp(mode, "variable-inspection-malformed") == 0) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"bad\",\"value\":42,\"variablesReference\":0}]}");
    if (strcmp(mode, "variable-inspection-unicode") == 0) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"caf\\u00e9\",\"value\":\"\\ud83d\\ude80\",\"variablesReference\":0}]}");
    if (strcmp(mode, "variable-inspection-cycle") == 0) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"parent\",\"value\":\"{...}\",\"variablesReference\":2}]}");
    if (strcmp(mode, "variable-inspection-event") == 0 && !Event("continued", "{\"threadId\":1}")) return 0;
    return Response(request->sequence, strcmp(mode, "variable-inspection-command") == 0 ? "evaluate" : "variables", 1,
        "{\"variables\":[{\"name\":\"item\",\"value\":\"{...}\",\"variablesReference\":3},{\"name\":\"item\",\"value\":\"other\",\"variablesReference\":0}]}");
}
