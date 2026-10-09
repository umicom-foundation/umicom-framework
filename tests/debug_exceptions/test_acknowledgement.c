/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exceptions/test_acknowledgement.c
 * PURPOSE: Distinguish successful protocol replies from per-filter verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/exception_filters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *body = NULL;
    bool good = false;
    size_t count = 1U;
    int verification = -1;
    if (strcmp(mode, "absent") == 0)
        good = true;
    else if (strcmp(mode, "unknown") == 0)
    {
        body = "{}";
        good = true;
    }
    else if (strcmp(mode, "verified") == 0)
    {
        body = "{\"breakpoints\":[{\"verified\":true}]}";
        good = true;
        verification = 1;
    }
    else if (strcmp(mode, "refused-filter") == 0)
    {
        body = "{\"breakpoints\":[{\"verified\":false,\"message\":\"Not available\"}]}";
        good = true;
        verification = 0;
    }
    else if (strcmp(mode, "empty") == 0)
    {
        body = "{\"breakpoints\":[]}";
        good = true;
        count = 0U;
    }
    else if (strcmp(mode, "count") == 0)
        body = "{\"breakpoints\":[]}";
    else if (strcmp(mode, "boolean") == 0)
        body = "{\"breakpoints\":[{\"verified\":1}]}";
    else if (strcmp(mode, "missing-verified") == 0)
        body = "{\"breakpoints\":[{}]}";
    else if (strcmp(mode, "nul") == 0)
        body = "{\"breakpoints\":[{\"verified\":false,\"message\":\"no\\u0000yes\"}]}";
    else if (strcmp(mode, "duplicate") == 0)
        body = "{\"breakpoints\":[{\"verified\":false,\"verified\":true}]}";
    else if (strcmp(mode, "null") == 0)
        body = "null";
    else if (strcmp(mode, "array") == 0)
        body = "{\"breakpoints\":{}}";
    else if (strcmp(mode, "refuse") == 0 || strcmp(mode, "command") == 0)
        body = "{}";
    else
        return 2;
    int failed = 0;
    UmiDebugExceptionAcknowledgement *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    CHECK(out != NULL && before != NULL);
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    char json[4096];
    snprintf(json, sizeof json, "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s%s%s}",
             strcmp(mode, "command") == 0 ? "launch" : "setExceptionBreakpoints",
             strcmp(mode, "refuse") == 0 ? "false" : "true", body ? ",\"body\":" : "",
             body ? body : "");
    UmiStatus status = UmiDebugExceptionAcknowledgementDecode(json, count, out);
    if (good)
    {
        CHECK(status == UMI_STATUS_OK && out->count == count);
        if (count)
            CHECK(out->verified[0] == verification);
        if (strcmp(mode, "refused-filter") == 0)
            CHECK(strcmp(out->message[0], "Not available") == 0);
    }
    else
        CHECK(status != UMI_STATUS_OK && memcmp(out, before, sizeof *out) == 0);
done:
    free(out);
    free(before);
    return failed;
}
