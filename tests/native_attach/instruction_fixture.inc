/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/instruction_fixture.inc
 * PURPOSE: Supply inert instruction pages and controlled failures to native debugger tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* These replies describe an imaginary stopped program. The child never opens a
 * real process. Tests use the transcript to verify request arguments and to
 * prove that instruction inspection never sends evaluation or execution work. */
static int InstructionFixtureCommand(const char *mode, const UmiDebugRuntimeEnvelope *request,
                                     const char *json, int *sent)
{
    const char *command = request->command;
    if (strcmp(command, "initialize") == 0)
    {
        *sent = Reply(request->sequence, command, 1,
                      strcmp(mode, "instructions-unsupported") == 0
                          ? "{\"supportsConfigurationDoneRequest\":true}"
                          : "{\"supportsConfigurationDoneRequest\":true,"
                            "\"supportsDisassembleRequest\":true}");
        return 1;
    }
    if (strcmp(command, "threads") == 0)
    {
        *sent = Reply(request->sequence, command, 1,
                      "{\"threads\":[{\"id\":1,\"name\":\"fixture thread\"}]}");
        return 1;
    }
    if (strcmp(command, "scopes") == 0)
    {
        *sent = Reply(request->sequence, command, 1, "{\"scopes\":[]}");
        return 1;
    }
    if (strcmp(command, "stackTrace") != 0 && strcmp(command, "disassemble") != 0)
        return 0;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    *sent = 0;
    if (document == NULL)
        return 1;
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int arguments = status == UMI_STATUS_OK
                        ? umi_language_runtime_json_object_get(document, 0, "arguments")
                        : -1;
    int64_t count = 0;
    int token = status == UMI_STATUS_OK
                    ? umi_language_runtime_json_object_get(
                          document, arguments,
                          strcmp(command, "stackTrace") == 0 ? "levels" : "instructionCount")
                    : -1;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_json_int64(document, token, &count);
    free(document);
    if (status != UMI_STATUS_OK)
        return 1;
    if (strcmp(command, "stackTrace") == 0)
    {
        /* Refuse a later malformed full refresh without changing a valid capture. */
        static unsigned full_stack_reads = 0U;
        if (count > 1 && ++full_stack_reads > 1U && strcmp(mode, "instructions-stack-invalid") == 0)
        {
            *sent = Reply(request->sequence, command, 1,
                          "{\"stackFrames\":[{\"id\":0,\"name\":\"valid\"},{\"id\":-1,\"name\":"
                          "\"invalid\"}]}");
            return 1;
        }
        /* The host first refreshes the full stack. Capture then asks for one
         * selected frame; fault injection affects that second operation only. */
        if (count == 1 && strcmp(mode, "instructions-capture-timeout") == 0)
        {
            *sent = 1;
            return 1;
        }
        if (count == 1 && strcmp(mode, "instructions-capture-refuse") == 0)
        {
            *sent = Reply(request->sequence, command, 0, "{}");
            return 1;
        }
        if (count == 1 && strcmp(mode, "instructions-capture-event") == 0 &&
            !Event("continued", "{\"threadId\":1,\"allThreadsContinued\":true}"))
            return 1;
        const char *frame =
            count == 1 && strcmp(mode, "instructions-missing-reference") == 0
                ? "{\"id\":0,\"name\":\"fixture frame\",\"line\":7,\"column\":1}"
            : count == 1 && strcmp(mode, "instructions-changed-frame") == 0
                ? "{\"id\":1,\"name\":\"other "
                  "frame\",\"line\":7,\"column\":1,\"instructionPointerReference\":\"0x1000\"}"
                : "{\"id\":0,\"name\":\"fixture "
                  "frame\",\"line\":7,\"column\":1,\"instructionPointerReference\":\"0x1000\"}";
        char body[512];
        int bytes = snprintf(body, sizeof body, "{\"stackFrames\":[%s],\"totalFrames\":1}", frame);
        if (bytes > 0 && (size_t)bytes < sizeof body)
            *sent = Reply(request->sequence, command, 1, body);
        return 1;
    }
    if (strcmp(mode, "instructions-timeout") == 0)
    {
        *sent = 1;
        return 1;
    }
    if (strcmp(mode, "instructions-refuse") == 0)
    {
        *sent = Reply(request->sequence, command, 0, "{}");
        return 1;
    }
    if (strcmp(mode, "instructions-malformed") == 0)
    {
        *sent = Reply(request->sequence, command, 1,
                      "{\"instructions\":[{\"address\":\"0x1000\",\"instruction\":false}]}");
        return 1;
    }
    if (strcmp(mode, "instructions-continued") == 0 &&
        !Event("continued", "{\"threadId\":1,\"allThreadsContinued\":true}"))
        return 1;
    if (count < 1 || count > 64)
        return 1;
    if (strcmp(mode, "instructions-short") == 0)
        --count;
    char *body = malloc(32768U);
    if (body == NULL)
        return 1;
    size_t used = (size_t)snprintf(body, 32768U, "{\"instructions\":[");
    for (int64_t i = 0; i < count; ++i)
    {
        int bytes =
            snprintf(body + used, 32768U - used,
                     "%s{\"address\":\"0x%llx\",\"instruction\":\"%s\",\"instructionBytes\":\"90\","
                     "\"symbol\":\"fixture_symbol\",\"presentationHint\":\"%s\"%s}",
                     i ? "," : "", (unsigned long long)(4096 + i), i == 1 ? "unavailable" : "nop",
                     i == 1 ? "invalid" : "normal",
                     i == 0 ? ",\"location\":{\"name\":\"fixture.c\",\"path\":\"/untrusted/"
                              "fixture.c\"},\"line\":7"
                            : "");
        if (bytes < 1 || (size_t)bytes >= 32768U - used)
        {
            free(body);
            return 1;
        }
        used += (size_t)bytes;
    }
    int bytes = snprintf(body + used, 32768U - used, "]}");
    if (bytes > 0 && (size_t)bytes < 32768U - used)
        *sent = Reply(request->sequence, command, 1, body);
    free(body);
    return 1;
}
