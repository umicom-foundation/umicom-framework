/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exception_information/test_decode.c
 * PURPOSE: Check nested exception detail ownership, limits and unchanged outputs on invalid peer data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/exception_inspection.h"
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
    const char *mode = argv[1], *body = NULL, *command = "exceptionInfo";
    int success = 1;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "normal") == 0)
        body = "{\"exceptionId\":\"error\",\"breakMode\":\"always\",\"details\":{\"message\":"
               "\"outer\",\"innerException\":[{\"message\":\"first\"},{\"message\":\"second\","
               "\"innerException\":[{\"message\":\"leaf\"}]}]}}";
    else if (strcmp(mode, "minimal") == 0)
        body = "{\"exceptionId\":\"error\",\"breakMode\":\"unhandled\"}";
    else if (strcmp(mode, "empty-detail") == 0)
        body = "{\"exceptionId\":\"error\",\"breakMode\":\"never\",\"details\":{}}";
    else if (strcmp(mode, "unknown-mode") == 0)
        body = "{\"exceptionId\":\"error\",\"breakMode\":\"adapter-mode\"}";
    else if (strcmp(mode, "unicode") == 0)
        body = "{\"exceptionId\":\"caf\\u00e9\",\"breakMode\":\"always\",\"description\":"
               "\"\\uD83D\\uDE00\"}";
    else if (strcmp(mode, "expression") == 0)
        body = "{\"exceptionId\":\"error\",\"breakMode\":\"always\",\"details\":{\"evaluateName\":"
               "\"dangerous()\",\"stackTrace\":\"<script>bad()</script>\\n\"}}";
    else if (strcmp(mode, "escaped-key") == 0)
        body = "{\"exception\\u0049d\":\"error\",\"breakMode\":\"always\"}";
    else if (strcmp(mode, "missing-id") == 0)
    {
        body = "{\"breakMode\":\"always\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "missing-mode") == 0)
    {
        body = "{\"exceptionId\":\"error\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "id-type") == 0)
    {
        body = "{\"exceptionId\":false,\"breakMode\":\"always\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "detail-type") == 0)
    {
        body = "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":false}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "child-type") == 0)
    {
        body = "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":{\"innerException\":["
               "false]}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "children-type") == 0)
    {
        body =
            "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":{\"innerException\":{}}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "nul") == 0)
    {
        body = "{\"exceptionId\":\"e\\u0000x\",\"breakMode\":\"always\"}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        body = "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":{\"message\":\"a\","
               "\"m\\u0065ssage\":\"b\"}}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "refuse") == 0)
    {
        body = "{}";
        success = 0;
        expected = UMI_STATUS_UNAVAILABLE;
    }
    else if (strcmp(mode, "command") == 0)
    {
        body = "{}";
        command = "evaluate";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "depth") == 0 || strcmp(mode, "capacity") == 0 ||
             strcmp(mode, "message-limit") == 0)
    {
    }
    else
        return 2;
    int failed = 0;
    char json[16384], generated[12000];
    UmiDebugExceptionInformation *out = malloc(sizeof *out), *before = malloc(sizeof *before);
    CHECK(out != NULL && before != NULL);
    memset(out, 0xA5, sizeof *out);
    *before = *out;
    if (strcmp(mode, "depth") == 0)
    {
        strcpy(generated, "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":");
        for (unsigned i = 0U; i < UMI_DEBUG_EXCEPTION_DEPTH_LIMIT; ++i)
            strcat(generated, "{\"innerException\":[");
        strcat(generated, "{}");
        for (unsigned i = 0U; i < UMI_DEBUG_EXCEPTION_DEPTH_LIMIT; ++i)
            strcat(generated, "]}");
        strcat(generated, "}");
        body = generated;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "capacity") == 0)
    {
        strcpy(generated,
               "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":{\"innerException\":[");
        for (size_t i = 0U; i < UMI_DEBUG_EXCEPTION_DETAIL_LIMIT; ++i)
            strcat(generated, i ? ",{}" : "{}");
        strcat(generated, "]}}");
        body = generated;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "message-limit") == 0)
    {
        strcpy(generated,
               "{\"exceptionId\":\"e\",\"breakMode\":\"always\",\"details\":{\"message\":\"");
        size_t offset = strlen(generated);
        memset(generated + offset, 'x', 2048U);
        strcpy(generated + offset + 2048U, "\"}}");
        body = generated;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    snprintf(json, sizeof json,
             "{\"type\":\"response\",\"command\":\"%s\",\"success\":%s,\"body\":%s}", command,
             success ? "true" : "false", body);
    CHECK(UmiDebugExceptionInformationDecode(json, out) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(out, before, sizeof *out) == 0);
    else
    {
        if (strcmp(mode, "normal") == 0)
        {
            CHECK(out->count == 4U && out->details[0].parent == SIZE_MAX);
            CHECK(out->details[1].parent == 0U && out->details[2].parent == 0U &&
                  out->details[3].parent == 2U);
            CHECK(out->details[3].depth == 2U && strcmp(out->details[3].message, "leaf") == 0);
        }
        if (strcmp(mode, "minimal") == 0)
            CHECK(out->count == 0U);
        if (strcmp(mode, "unicode") == 0)
            CHECK(strcmp(out->exception_id, "caf\xc3\xa9") == 0);
        if (strcmp(mode, "expression") == 0)
            CHECK(strcmp(out->details[0].evaluate_name, "dangerous()") == 0 &&
                  strstr(out->details[0].stack_trace, "<script>") != NULL);
    }
done:
    free(out);
    free(before);
    return failed;
}
