/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/peer.inc
 * PURPOSE: Emit deliberate watch responses from the deterministic DAP process fixture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static int WatchPeerReply(const UmiDebugRuntimeEnvelope *request, const char *mode, const char *json)
{
    if (strstr(json, "\"frameId\":0") == NULL || strstr(json, "\"context\":\"watch\"") == NULL ||
        strstr(json, "\"expression\":\"savedNotes\"") == NULL) return 0;
    if (strcmp(mode, "watch-edit-rejected") == 0) return Response(request->sequence, "evaluate", 0, "{}");
    if (strcmp(mode, "watch-edit-malformed") == 0) return Response(request->sequence, "evaluate", 1, "{\"result\":42}");
    if (strcmp(mode, "watch-edit-empty") == 0) return Response(request->sequence, "evaluate", 1, "{\"result\":\"\"}");
    if (strcmp(mode, "watch-edit-unicode") == 0) return Response(request->sequence, "evaluate", 1, "{\"result\":\"caf\\u00e9 \\ud83d\\ude80\",\"type\":\"text\"}");
    if (strcmp(mode, "watch-edit-event") == 0 && !Event("continued", "{\"threadId\":1}")) return 0;
    if (strcmp(mode, "watch-edit-long") == 0) {
        char value[1025], body[1100]; memset(value, 'x', 1024U); value[1024] = '\0';
        (void)snprintf(body, sizeof body, "{\"result\":\"%s\"}", value);
        return Response(request->sequence, "evaluate", 1, body);
    }
    return Response(request->sequence, strcmp(mode, "watch-edit-command") == 0 ? "variables" : "evaluate",
        1, "{\"result\":\"2\",\"type\":\"int\"}");
}
