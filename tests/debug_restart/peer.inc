/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_restart/peer.inc
 * PURPOSE: Supply controlled restart capability, failure and event ordering replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* A restart can require configuration before its response is sent. These
 * counters belong only to the controlled peer, never to the runtime under test. */
static uint64_t RestartPeerPending;
static int RestartPeerAcknowledged, RestartPeerBreakpointRequests;
static int RestartPeerReply(const UmiDebugRuntimeEnvelope *request, const char *mode)
{
    if (strcmp(request->command, "setBreakpoints") == 0)
    {
        if (RestartPeerPending == 0U || strstr(request->json, "\"line\":13") == NULL)
            return 0;
        ++RestartPeerBreakpointRequests;
        return Response(request->sequence, "setBreakpoints", 1,
                        "{\"breakpoints\":[{\"id\":1,\"verified\":true,\"line\":13}]}");
    }
    if (strcmp(request->command, "configurationDone") == 0)
    {
        if (RestartPeerPending == 0U || RestartPeerBreakpointRequests != 1)
            return 0;
        if (strcmp(mode, "session-restart-reconfigure-timeout") == 0)
            return 1;
        if (strcmp(mode, "session-restart-reconfigure-rejected") == 0)
        {
            RestartPeerPending = 0U;
            return Response(request->sequence, "configurationDone", 0, "{}");
        }
        if (!RestartPeerAcknowledged && !Response(RestartPeerPending, "restart", 1, "{}"))
            return 0;
        RestartPeerPending = 0U;
        return Response(request->sequence, "configurationDone", 1, "{}") &&
               Event("stopped", "{\"reason\":\"entry\",\"threadId\":1,\"allThreadsStopped\":true}");
    }
    if (strcmp(request->command, "restart") == 0 &&
        strncmp(mode, "session-restart-reconfigure", strlen("session-restart-reconfigure")) == 0)
    {
        RestartPeerPending = request->sequence;
        RestartPeerAcknowledged = strcmp(mode, "session-restart-reconfigure-after") == 0;
        RestartPeerBreakpointRequests = 0;
        if (RestartPeerAcknowledged && !Response(request->sequence, "restart", 1, "{}"))
            return 0;
        return Event("initialized", "{}");
    }
    if (strcmp(request->command, "initialize") == 0)
        return Response(request->sequence, request->command, 1,
                        strcmp(mode, "session-restart-unsupported") == 0
                            ? "{\"supportsConfigurationDoneRequest\":true}"
                            : "{\"supportsConfigurationDoneRequest\":true,\"supportsRestartRequest\":true}");
    if (strcmp(mode, "session-restart-unsupported") == 0 || strcmp(mode, "session-restart-zero") == 0 ||
        strcmp(mode, "session-restart-queued") == 0)
        return 0;
    if (strcmp(mode, "session-restart-rejected") == 0)
        return Response(request->sequence, "restart", 0, "{}");
    if (strcmp(mode, "session-restart-timeout") == 0)
        return 1;
    if (strcmp(mode, "session-restart-wrong-command") == 0)
        return Response(request->sequence, "continue", 1, "{}");
    const char *stopped = "{\"reason\":\"entry\",\"threadId\":1,\"allThreadsStopped\":true}";
    if (strcmp(mode, "session-restart-event-first") == 0)
        return Event("stopped", stopped) && Response(request->sequence, "restart", 1, "{}");
    return Response(request->sequence, "restart", 1, "{}") && Event("stopped", stopped);
}
