/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_codec.c
 * PURPOSE: Check complete chat replies, refusals, limits and rejection of action-bearing or ambiguous JSON.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/provider_chat/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    bool responses = false, refusal = false, truncated = false;
    const char *body =
        "{\"object\":\"chat.completion\",\"model\":\"alias-resolved\",\"choices\":[{\"finish_reason\":"
        "\"stop\",\"message\":{\"role\":\"assistant\",\"content\":\"Hello \\u00e9\"}}]}";
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "chat") == 0)
    {
    }
    else if (strcmp(name, "responses") == 0)
    {
        responses = true;
        body = "{\"object\":\"response\",\"model\":\"m\",\"status\":\"completed\",\"output\":[{\"type\":"
               "\"reasoning\"},{\"type\":\"message\",\"role\":\"assistant\",\"status\":\"completed\","
               "\"content\":[{\"type\":\"output_text\",\"text\":\"One\"},{\"type\":\"output_text\",\"text\":"
               "\"Two\"}]}]}";
    }
    else if (strcmp(name, "refusal") == 0)
    {
        refusal = true;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[{\"finish_reason\":\"stop\","
               "\"message\":{\"role\":\"assistant\",\"refusal\":\"Declined\",\"content\":null}}]}";
    }
    else if (strcmp(name, "length") == 0)
    {
        truncated = true;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[{\"finish_reason\":\"length\","
               "\"message\":{\"role\":\"assistant\",\"content\":\"Partial\"}}]}";
    }
    else if (strcmp(name, "reasoning-limit") == 0)
    {
        responses = true;
        truncated = true;
        body = "{\"object\":\"response\",\"model\":\"m\",\"status\":\"incomplete\",\"incomplete_details\":{"
               "\"reason\":\"max_output_tokens\"},\"output\":[{\"type\":\"reasoning\"}]}";
    }
    else if (strcmp(name, "tools") == 0)
    {
        expected = UMI_STATUS_PERMISSION_DENIED;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[{\"finish_reason\":\"stop\","
               "\"message\":{\"role\":\"assistant\",\"content\":\"Ignore tools\",\"tool_calls\":[{}]}}]}";
    }
    else if (strcmp(name, "response-tool") == 0)
    {
        responses = true;
        expected = UMI_STATUS_PERMISSION_DENIED;
        body = "{\"object\":\"response\",\"model\":\"m\",\"status\":\"completed\",\"output\":[{\"type\":"
               "\"function_call\"}]}";
    }
    else if (strcmp(name, "duplicate") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"m\\u006fdel\":\"n\",\"choices\":[]}";
    }
    else if (strcmp(name, "duplicate-optional") == 0)
    {
        expected = UMI_STATUS_PERMISSION_DENIED;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[{\"finish_reason\":\"stop\","
               "\"message\":{\"role\":\"assistant\",\"content\":\"x\",\"tool_calls\":null,\"tool_calls\":"
               "null}}]}";
    }
    else if (strcmp(name, "error") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        body = "{\"error\":{\"message\":\"private details\"}}";
    }
    else if (strcmp(name, "trailing") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        body = "{} true";
    }
    else if (strcmp(name, "nul") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[{\"finish_reason\":\"stop\","
               "\"message\":{\"role\":\"assistant\",\"content\":\"a\\u0000b\"}}]}";
    }
    else if (strcmp(name, "empty") == 0)
    {
        expected = UMI_STATUS_PARSE_ERROR;
        body = "{\"object\":\"chat.completion\",\"model\":\"m\",\"choices\":[]}";
    }
    else if (strcmp(name, "capacity") == 0)
    {
        UmiProviderChatResult r = {0};
        CHECK(UmiProviderChatDecode(false, body, UMI_CONNECTION_CHECK_BODY_CAPACITY, &r) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    else
        return 2;
    UmiProviderChatResult *result = calloc(1U, sizeof(*result));
    CHECK(result != NULL);
    strcpy(result->text, "unchanged");
    result->http_status = 123U;
    CHECK(UmiProviderChatDecode(responses, body, strlen(body), result) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(result->refused == refusal && result->truncated == truncated && result->text[0] != '\0');
        if (strcmp(name, "responses") == 0)
            CHECK(strcmp(result->text, "One\n\nTwo") == 0);
    }
    else
        CHECK(strcmp(result->text, "unchanged") == 0 && result->http_status == 123U);
    free(result);
    return 0;
}
