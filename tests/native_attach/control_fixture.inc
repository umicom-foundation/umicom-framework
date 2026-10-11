/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/control_fixture.inc
 * PURPOSE: Provide deterministic control acknowledgements without resuming a real target.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The fixture is a protocol peer only. Each test records whether one control
 * command was sent, then chooses an acknowledgement or a lifecycle event. */
static int ControlFixtureCommand(const char *mode, const UmiDebugRuntimeEnvelope *request,
                                 const char *json, int *sent)
{
    const char *command = request->command;
    if (strcmp(command, "initialize") == 0)
    {
        *sent =
            Reply(request->sequence, command, 1,
                  "{\"supportsConfigurationDoneRequest\":true,\"supportsDisassembleRequest\":true,"
                  "\"supportsStepBack\":true}");
        return 1;
    }
    bool resume = strcmp(command, "continue") == 0 || strcmp(command, "next") == 0 ||
                  strcmp(command, "stepIn") == 0 || strcmp(command, "stepOut") == 0 ||
                  strcmp(command, "stepBack") == 0 || strcmp(command, "reverseContinue") == 0;
    if (!resume && strcmp(command, "pause") != 0)
        return InstructionFixtureCommand("instructions-normal", request, json, sent);
    if (strcmp(mode, "control-timeout") == 0)
    {
        *sent = 1;
        return 1;
    }
    if (strcmp(mode, "control-refuse") == 0)
    {
        *sent = Reply(request->sequence, command, 0, "{}");
        return 1;
    }
    if (strcmp(mode, "control-early-stop") == 0 &&
        !Event("stopped", "{\"reason\":\"step\",\"threadId\":1,\"allThreadsStopped\":true}"))
    {
        *sent = 0;
        return 1;
    }
    if (strcmp(mode, "control-bodyless") == 0)
    {
        char reply[256];
        int bytes = snprintf(reply, sizeof reply,
                             "{\"seq\":%llu,\"type\":\"response\",\"request_seq\":%llu,"
                             "\"success\":true,\"command\":\"next\"}",
                             (unsigned long long)sequence++, (unsigned long long)request->sequence);
        *sent = bytes > 0 && (size_t)bytes < sizeof reply && Send(reply);
        return 1;
    }
    const char *body = strcmp(mode, "control-bad-body") == 0  ? "{\"allThreadsContinued\":\"yes\"}"
                       : strcmp(mode, "control-partial") == 0 ? "{\"allThreadsContinued\":false}"
                                                              : "{}";
    *sent = Reply(request->sequence,
                  strcmp(mode, "control-wrong-command") == 0 ? "unrelated" : command, 1, body);
    if (*sent && (strcmp(mode, "control-late-stop") == 0 ||
                  (strcmp(command, "pause") == 0 && strcmp(mode, "control-pause") == 0)))
        *sent = Event("stopped", "{\"reason\":\"step\",\"threadId\":1,\"allThreadsStopped\":true}");
    return 1;
}
