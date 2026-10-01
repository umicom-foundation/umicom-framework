/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/scope_inspection/peer.inc
 * PURPOSE: Provide unloaded expensive scopes and request-sensitive child values through the DAP fixture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static int ScopePeerScopes(const UmiDebugRuntimeEnvelope *request, const char *mode)
{
    if (strcmp(mode, "scope-inspection-only-expensive") == 0)
        return Response(request->sequence, "scopes", 1,
            "{\"scopes\":[{\"name\":\"Globals\",\"variablesReference\":9,\"expensive\":true}]}");
    return Response(request->sequence, "scopes", 1,
        "{\"scopes\":[{\"name\":\"Locals\",\"variablesReference\":1,\"expensive\":false},"
        "{\"name\":\"Globals\",\"variablesReference\":9,\"expensive\":true},"
        "{\"name\":\"Globals\",\"variablesReference\":10,\"expensive\":true}]}");
}
static int ScopePeerVariables(const UmiDebugRuntimeEnvelope *request, const char *mode, const char *json)
{
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    if (doc == NULL) return 0;
    int64_t reference = -1; UmiStatus status = umi_language_runtime_json_parse(json, doc);
    if (status == UMI_STATUS_OK) {
        int arguments = umi_language_runtime_json_object_get(doc, 0, "arguments");
        status = umi_language_runtime_json_int64(doc,
            umi_language_runtime_json_object_get(doc, arguments, "variablesReference"), &reference);
    }
    free(doc); if (status != UMI_STATUS_OK) return 0;
    if (reference == 1) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"local\",\"value\":\"2\",\"variablesReference\":0}]}");
    if (reference == 10) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"other\",\"value\":\"99\",\"variablesReference\":0}]}");
    if (strcmp(mode, "scope-inspection-budget") == 0 && (reference == 9 || reference == 11)) {
        char body[8192]; strcpy(body, "{\"variables\":[");
        for (size_t i = 0U; i < 128U; ++i) strcat(body, i == 0U ?
            "{\"name\":\"i\",\"value\":\"v\",\"variablesReference\":11}" : ",{\"name\":\"i\",\"value\":\"v\",\"variablesReference\":11}");
        strcat(body, "]}"); return Response(request->sequence, "variables", 1, body);
    }
    if (reference == 11) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"leaf\",\"value\":\"7\",\"variablesReference\":0}]}");
    if (reference != 9) return 0;
    static unsigned captures = 0U; ++captures;
    if (strcmp(mode, "scope-inspection-timeout") == 0) return 1;
    if (strcmp(mode, "scope-inspection-rejected") == 0) return Response(request->sequence, "variables", 0, "{}");
    if (strcmp(mode, "scope-inspection-malformed") == 0 ||
        (strcmp(mode, "scope-inspection-failed-refresh") == 0 && captures > 1U))
        return Response(request->sequence, "variables", 1, "{\"variables\":[{\"name\":\"broken\"}]}");
    if (strcmp(mode, "scope-inspection-empty") == 0) return Response(request->sequence, "variables", 1, "{\"variables\":[]}");
    if (strcmp(mode, "scope-inspection-cycle") == 0) return Response(request->sequence, "variables", 1,
        "{\"variables\":[{\"name\":\"ancestor\",\"value\":\"{...}\",\"variablesReference\":9}]}");
    if (strcmp(mode, "scope-inspection-event") == 0 && !Event("continued", "{\"threadId\":1}")) return 0;
    char body[512];
    (void)snprintf(body, sizeof body,
        "{\"variables\":[{\"name\":\"settings\",\"value\":\"capture %u\",\"variablesReference\":11}]}", captures);
    return Response(request->sequence, "variables", 1, body);
}
