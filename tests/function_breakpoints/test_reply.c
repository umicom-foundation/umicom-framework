/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/function_breakpoints/test_reply.c
 * PURPOSE: Reject ambiguous or truncated function-breakpoint verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/function_breakpoint_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *rows = NULL;
    bool good = false;
    size_t count = 1U;
    if (strcmp(mode, "verified") == 0)
    {
        rows = "[{\"id\":17,\"verified\":true,\"source\":{\"path\":\"/project/"
               "main.c\"},\"line\":8,\"column\":2}]";
        good = true;
    }
    else if (strcmp(mode, "unverified") == 0)
    {
        rows = "[{\"verified\":false,\"message\":\"missing symbols\"}]";
        good = true;
    }
    else if (strcmp(mode, "empty") == 0)
    {
        rows = "[]";
        count = 0U;
        good = true;
    }
    else if (strcmp(mode, "count") == 0)
        rows = "[]";
    else if (strcmp(mode, "missing") == 0)
        rows = "[{}]";
    else if (strcmp(mode, "duplicate") == 0)
        rows = "[{\"verified\":true,\"verified\":false}]";
    else if (strcmp(mode, "nul") == 0)
        rows = "[{\"verified\":false,\"message\":\"no\\u0000yes\"}]";
    else if (strcmp(mode, "line-zero") == 0)
        rows = "[{\"verified\":true,\"line\":0}]";
    else if (strcmp(mode, "line-overflow") == 0)
        rows = "[{\"verified\":true,\"line\":2147483648}]";
    else if (strcmp(mode, "negative-id") == 0)
        rows = "[{\"verified\":true,\"id\":-1}]";
    else if (strcmp(mode, "source") == 0)
        rows = "[{\"verified\":true,\"source\":\"bad\"}]";
    else if (strcmp(mode, "refuse") == 0 || strcmp(mode, "command") == 0)
        rows = "[]";
    else
        return 2;
    UmiDebugFunctionReply *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    if (out == NULL || before == NULL)
    {
        free(out);
        free(before);
        return 1;
    }
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    char json[4096];
    snprintf(
        json, sizeof json,
        "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s,\"body\":{\"breakpoints\":%s}}",
        strcmp(mode, "command") == 0 ? "launch" : "setFunctionBreakpoints",
        strcmp(mode, "refuse") == 0 ? "false" : "true", rows);
    UmiStatus status = UmiDebugFunctionReplyDecode(json, count, out);
    bool okay = good ? status == UMI_STATUS_OK && out->count == count
                     : status != UMI_STATUS_OK && memcmp(out, before, sizeof *out) == 0;
    if (okay && strcmp(mode, "verified") == 0)
        okay = out->entries[0].adapter_id == 17U && out->entries[0].line == 8U &&
               out->entries[0].column == 2U && out->entries[0].verified;
    if (okay && strcmp(mode, "unverified") == 0)
        okay = !out->entries[0].verified && strcmp(out->entries[0].message, "missing symbols") == 0;
    if (!okay)
        fprintf(stderr, "Unexpected reply for %s: %s\n", mode, umi_status_text(status));
    free(out);
    free(before);
    return okay ? 0 : 1;
}
