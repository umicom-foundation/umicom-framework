/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/call_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for prepared call identities, individual sites and native cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
static int CallHierarchyFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], current_uri[4096] = "\"file:///workspace/main.c\"";
    int stage = 0, result = 0;
    unsigned followups = 0U, roots = 1U;
    char items[2][6144];
    for (;;)
    {
        size_t length = 0U;
        int have_length = 0;
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        for (;;)
        {
            if (strncmp(line, "Content-Length: ", 16U) == 0)
            {
                char *end = NULL;
                unsigned long value = strtoul(line + 16U, &end, 10);
                if (end == line + 16U || (*end != '\r' && *end != '\n') || value >= 65536UL)
                {
                    result = 4;
                    goto done;
                }
                length = (size_t)value;
                have_length = 1;
            }
            if (strcmp(line, "\r\n") == 0)
                break;
            if (fgets(line, sizeof(line), stdin) == NULL)
            {
                result = 4;
                goto done;
            }
        }
        if (!have_length || fread(body, 1U, length, stdin) != length)
        {
            result = 4;
            break;
        }
        body[length] = '\0';
        const char *id_text = strstr(body, "\"id\":");
        unsigned long long id = id_text == NULL ? 0ULL : strtoull(id_text + 5U, NULL, 10);
        int written = 0;
        if (strstr(body, "\"method\":\"initialize\"") != NULL)
        {
            if (stage != 0 || strstr(body, "\"callHierarchy\":{\"dynamicRegistration\":false}") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps =
                strcmp(mode, "calls-encoding") == 0
                    ? "\"callHierarchyProvider\":true,\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\""
                    : "\"callHierarchyProvider\":true,\"textDocumentSync\":{\"openClose\":true,\"change\":"
                      "2}";
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{%s}}}", id, caps);
        }
        else if (strstr(body, "\"method\":\"initialized\"") != NULL)
        {
            if (stage != 1)
            {
                result = 5;
                break;
            }
            stage = 2;
        }
        else if (strstr(body, "\"method\":\"textDocument/didOpen\"") != NULL)
        {
            if (stage != 2 || strstr(body, "\"text\":") == NULL || strstr(body, "\"version\":1") == NULL)
            {
                result = 5;
                break;
            }
            if (strcmp(mode, "calls-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
            {
                result = 6;
                break;
            }
            /* Capture the exact request document even for anonymous drafts. */
            {
                /* Keep the complete quoted URI from our known didOpen shape.
                 * This independent peer must echo the test document identity,
                 * including escaped bytes, without using the product decoder. */
                const char *uri = strstr(body, "\"uri\":");
                if (uri == NULL || uri[6] != '"')
                {
                    result = 6;
                    break;
                }
                uri += 6;
                const char *end = uri + 1;
                while (*end != '\0' && *end != '"')
                {
                    if (*end == '\\' && end[1] != '\0')
                        ++end;
                    ++end;
                }
                size_t count = (size_t)(end - uri) + 1U;
                if (*end != '"' || count >= sizeof(current_uri))
                {
                    result = 6;
                    break;
                }
                memcpy(current_uri, uri, count);
                current_uri[count] = '\0';
            }
            stage = 3;
        }
        else if (strstr(body, "\"method\":\"textDocument/prepareCallHierarchy\"") != NULL)
        {
            if (stage != 3 || strstr(body, current_uri) == NULL || strstr(body, "\"position\":") == NULL)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "calls-empty") == 0)
                roots = 0U;
            if (strcmp(mode, "calls-multiple") == 0 || strcmp(mode, "calls-second-error") == 0)
                roots = 2U;
            const char *root_uri =
                strcmp(mode, "calls-external-root") == 0 ? "\"file:///workspace/definition.c\"" : current_uri;
            unsigned end =
                strcmp(mode, "calls-unicode") == 0 || strcmp(mode, "calls-surrogate") == 0 ? 4U : 2U;
            if (strcmp(mode, "calls-outside-root") == 0)
                end = 9U;
            for (unsigned i = 0U; i < roots; ++i)
            {
                int count = snprintf(items[i], sizeof(items[i]),
                                     "{\"name\":\"root%u\",\"kind\":12,\"uri\":%s,"
                                     "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                     "\"character\":%u}},"
                                     "\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                                     "\"line\":0,\"character\":1}},"
                                     "\"data\":{\"nativeToken\":%u},\"extension\":{\"retain\":true}}",
                                     i, root_uri, end, i + 7U);
                if (count <= 0 || (size_t)count >= sizeof(items[i]))
                {
                    result = 6;
                    goto done;
                }
            }
            if (strcmp(mode, "calls-prepare-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"no symbol\"}}",
                    id);
            else if (strcmp(mode, "calls-invalid-root") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{}]}", id);
            else if (roots == 0U)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[%s%s%s]}", id, items[0],
                                   roots == 2U ? "," : "", roots == 2U ? items[1] : "");
        }
        else if (strstr(body, "\"method\":\"callHierarchy/incomingCalls\"") != NULL ||
                 strstr(body, "\"method\":\"callHierarchy/outgoingCalls\"") != NULL)
        {
            int outgoing = strstr(body, "callHierarchy/outgoingCalls") != NULL;
            if (stage != 4 || followups >= roots || strstr(body, items[followups]) == NULL ||
                strstr(body, "\"params\":{\"item\":") == NULL)
            {
                result = 6;
                break;
            }
            if ((strcmp(mode, "calls-outgoing") == 0 || strcmp(mode, "calls-external-root") == 0) &&
                !outgoing)
            {
                result = 6;
                break;
            }
            ++followups;
            if (strcmp(mode, "calls-timeout") == 0)
                continue;
            if (strcmp(mode, "calls-disconnect") == 0)
                break;
            const char *uri =
                strcmp(mode, "calls-external-root") == 0 ? "\"file:///workspace/related.c\"" : current_uri;
            unsigned end =
                strcmp(mode, "calls-unicode") == 0 || strcmp(mode, "calls-surrogate") == 0 ? 4U : 2U;
            unsigned first = strcmp(mode, "calls-unicode") == 0     ? 3U
                             : strcmp(mode, "calls-surrogate") == 0 ? 2U
                                                                    : 1U;
            unsigned last = strcmp(mode, "calls-outside-site") == 0 ? 9U : end;
            if (strcmp(mode, "calls-error") == 0 ||
                (strcmp(mode, "calls-second-error") == 0 && followups == 2U))
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"no calls\"}}", id);
            else if (strcmp(mode, "calls-invalid") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{}]}", id);
            else if (strcmp(mode, "calls-no-calls") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[]}", id);
            else
            {
                char ranges[512];
                (void)snprintf(
                    ranges, sizeof(ranges),
                    "[{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":%u}},"
                    "{\"start\":{\"line\":0,\"character\":%u},\"end\":{\"line\":0,\"character\":%u}}]",
                    first, first, last);
                if (strcmp(mode, "calls-no-sites") == 0)
                    strcpy(ranges, "[]");
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"%s\":{"
                    "\"name\":\"related%u\",\"kind\":12,\"detail\":\"void related(void)\",\"uri\":%s,"
                    "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":%u}}"
                    ","
                    "\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                    "\"character\":1}}},"
                    "\"fromRanges\":%s}]}",
                    id, outgoing ? "to" : "from", followups, uri, end, ranges);
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if (stage != 4)
            {
                result = 5;
                break;
            }
            if ((strcmp(mode, "calls-incoming") == 0 || strcmp(mode, "calls-outgoing") == 0 ||
                 strcmp(mode, "calls-multiple") == 0 || strcmp(mode, "calls-empty") == 0 ||
                 strcmp(mode, "calls-no-calls") == 0) &&
                followups != roots)
            {
                result = 5;
                break;
            }
            stage = 5;
        }
        else if (strstr(body, "\"method\":\"shutdown\"") != NULL)
        {
            if (stage != 5)
            {
                result = 5;
                break;
            }
            stage = 6;
            if (strcmp(mode, "calls-shutdown") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":"
                                   "\"shutdown failed\"}}",
                                   id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
        }
        else if (strstr(body, "\"method\":\"exit\"") != NULL)
        {
            if (stage != 6)
                result = 5;
            break;
        }
        else
        {
            result = 6;
            break;
        }
        if (written > 0 &&
            ((size_t)written >= sizeof(response) || printf("Content-Length: %d\r\n\r\n", written) < 0 ||
             fwrite(response, 1U, (size_t)written, stdout) != (size_t)written || fflush(stdout) != 0))
        {
            result = 7;
            break;
        }
    }
done:
    free(body);
    return result;
}
