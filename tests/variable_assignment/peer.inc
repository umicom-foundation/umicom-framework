/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/variable_assignment/peer.inc
 * PURPOSE: Model assignment requests and uncertain replies without executing a user program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_text.h"
#include "umicom/language_runtime/json_writer.h"
static char AssignmentPeerValue[4096] = "2";
static int AssignmentPeerReply(const UmiDebugRuntimeEnvelope *request, const char *mode, const char *json)
{
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof *doc);
    if (doc == NULL) return 0;
    int good = umi_language_runtime_json_parse(json, doc) == UMI_STATUS_OK;
    int arguments = good ? umi_language_runtime_json_object_get(doc, 0, "arguments") : -1;
    int token = arguments >= 0 ? umi_language_runtime_json_object_get(doc, arguments, "variablesReference") : -1;
    int64_t reference = 0; char name[256] = {0}, value[4096] = {0};
    int expression = strcmp(request->command, "setExpression") == 0;
    if (!expression && (token < 0 || umi_language_runtime_json_int64(doc, token, &reference) != UMI_STATUS_OK)) good = 0;
    if (strcmp(request->command, "variables") == 0) {
        free(doc); if (!good || (reference != 1 && reference != 2)) return 0;
        if (reference == 2 && strcmp(mode, "variable-assignment-prequeued") == 0 &&
            !Event("output", "{\"category\":\"console\",\"output\":\"pending output\"}")) return 0;
        char body[8192]; UmiLanguageRuntimeJsonWriter writer;
        umi_language_runtime_json_writer_init(&writer, body, sizeof body);
        (void)umi_language_runtime_json_writer_raw(&writer, reference == 1 ?
            "{\"variables\":[{\"name\":\"notes\",\"variablesReference\":2,\"value\":" :
            "{\"variables\":[{\"name\":\"count\",\"variablesReference\":0,\"value\":");
        (void)umi_language_runtime_json_writer_string(&writer, AssignmentPeerValue);
        (void)umi_language_runtime_json_writer_raw(&writer, "}]}");
        return writer.status == UMI_STATUS_OK && Response(request->sequence, "variables", 1, body);
    }
    token = arguments >= 0 ? umi_language_runtime_json_object_get(doc, arguments, expression ? "expression" : "name") : -1;
    if (token < 0 || UmiLanguageRuntimeJsonText(doc, token, name, sizeof name) != UMI_STATUS_OK) good = 0;
    token = arguments >= 0 ? umi_language_runtime_json_object_get(doc, arguments, "value") : -1;
    if (token < 0 || UmiLanguageRuntimeJsonText(doc, token, value, sizeof value) != UMI_STATUS_OK) good = 0;
    free(doc);
    if (!good || (!expression && !((reference == 1 && strcmp(name, "notes") == 0) ||
        (reference == 2 && strcmp(name, "count") == 0)))) return 0;
    if (strcmp(mode, "variable-assignment-unsupported") == 0) return 0;
    if (strcmp(mode, "variable-assignment-rejected") == 0) return Response(request->sequence, request->command, 0, "{}");
    /* Persist the change before fault injection: a missing reply must never
     * persuade the client that retrying the mutation is harmless. */
    strcpy(AssignmentPeerValue, value);
    if (strcmp(mode, "variable-assignment-timeout") == 0) return 1;
    if (strcmp(mode, "variable-assignment-malformed") == 0) return Response(request->sequence, request->command, 1, "{\"result\":\"wrong field\"}");
    if (strcmp(mode, "variable-assignment-event") == 0 && !Event("continued", "{\"threadId\":1}")) return 0;
    char body[8192]; UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, body, sizeof body);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"value\":");
    (void)umi_language_runtime_json_writer_string(&writer, AssignmentPeerValue);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"type\":\"int\",\"variablesReference\":0}");
    return writer.status == UMI_STATUS_OK && Response(request->sequence,
        strcmp(mode, "variable-assignment-command") == 0 ? "evaluate" : request->command, 1, body);
}
