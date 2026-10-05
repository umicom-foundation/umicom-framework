/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/diagnostic_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native diagnostic publications and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
/* Add independent related-location publications without changing the existing
 * protocol ordering. The former peer is retained for engineering review. */
#if 0
static int DiagnosticFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[16384], uri[8192] = {0};
    int stage = 0, result = 0;
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
            if (stage != 0 || strstr(body, "\"versionSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "diagnostics-encoding") == 0 ? "\"textDocumentSync\":2,"
                                                                           "\"positionEncoding\":\"utf-8\""
                                                                         : "\"textDocumentSync\":{"
                                                                           "\"openClose\":true,\"change\":2}";
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
            if (strcmp(mode, "diagnostics-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
            {
                result = 6;
                break;
            }
            const char *start = strstr(body, "\"uri\":\"");
            if (start == NULL)
            {
                result = 6;
                break;
            }
            start += 6;
            const char *end = start + 1;
            while (*end != '\0' && *end != '"')
            {
                if (*end == '\\' && end[1] != '\0')
                    ++end;
                ++end;
            }
            if (*end != '"' || (size_t)(end - start) + 1U >= sizeof(uri))
            {
                result = 6;
                break;
            }
            size_t bytes = (size_t)(end - start) + 1U;
            memcpy(uri, start, bytes);
            uri[bytes] = '\0';

            stage = 4;
            if (strcmp(mode, "diagnostics-timeout") == 0)
                continue;
            if (strcmp(mode, "diagnostics-disconnect") == 0)
                break;
            const char *items =
                "[{\"message\":\"Unknown "
                "name\",\"severity\":2,\"source\":\"fixture\",\"code\":\"name\",\"range\":{\"start\":{"
                "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-empty") == 0)
                items = "[]";
            if (strcmp(mode, "diagnostics-invalid") == 0)
                items = "[{\"message\":\"missing range\"}]";
            if (strcmp(mode, "diagnostics-literal") == 0)
                items = "[{\"message\":\"<script>literal</"
                        "script>\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-outside") == 0 || strcmp(mode, "diagnostics-surrogate") == 0)
                items = "[{\"message\":\"invalid "
                        "endpoint\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":3}}}]";
            if (strcmp(mode, "diagnostics-related") == 0)
                items = "[{\"message\":\"name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                        "\"line\":0,\"character\":2}},\"relatedInformation\":[{\"message\":\"declared "
                        "here\",\"location\":{\"uri\":\"file:///"
                        "other.c\",\"range\":{\"start\":{\"line\":3,\"character\":0},\"end\":{\"line\":3,"
                        "\"character\":1}}}}]}]";
            if (strcmp(mode, "diagnostics-choices") == 0)
                items = "[{\"message\":\"First\",\"severity\":1,\"range\":{\"start\":{\"line\":0,"
                        "\"character\":0},\"end\":{\"line\":0,\"character\":1}}},{\"message\":\"Second\","
                        "\"severity\":4,\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":"
                        "0,\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-wrong-uri") == 0 || strcmp(mode, "diagnostics-stale") == 0)
            {
                int n = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                    "publishDiagnostics\",\"params\":{\"uri\":%s,\"version\":%d,\"diagnostics\":[]}}",
                    strcmp(mode, "diagnostics-wrong-uri") == 0 ? "\"file:///other.c\"" : uri,
                    strcmp(mode, "diagnostics-stale") == 0 ? 0 : 1);
                if (n <= 0 || (size_t)n >= sizeof(response) || printf("Content-Length: %d\r\n\r\n", n) < 0 ||
                    fwrite(response, 1U, (size_t)n, stdout) != (size_t)n || fflush(stdout) != 0)
                {
                    result = 7;
                    break;
                }
            }
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                         "publishDiagnostics\",\"params\":{\"uri\":%s,%s\"diagnostics\":%s}}",
                         uri, strcmp(mode, "diagnostics-unversioned") == 0 ? "" : "\"version\":1,", items);
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if (stage != 4)
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
            if (strcmp(mode, "diagnostics-shutdown") == 0)
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
#endif
static int DiagnosticFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[16384], uri[8192] = {0};
    int stage = 0, result = 0;
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
            if (stage != 0 || strstr(body, "\"versionSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "diagnostics-encoding") == 0 ? "\"textDocumentSync\":2,"
                                                                           "\"positionEncoding\":\"utf-8\""
                                                                         : "\"textDocumentSync\":{"
                                                                           "\"openClose\":true,\"change\":2}";
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
            if (strcmp(mode, "diagnostics-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
            {
                result = 6;
                break;
            }
            const char *start = strstr(body, "\"uri\":\"");
            if (start == NULL)
            {
                result = 6;
                break;
            }
            start += 6;
            const char *end = start + 1;
            while (*end != '\0' && *end != '"')
            {
                if (*end == '\\' && end[1] != '\0')
                    ++end;
                ++end;
            }
            if (*end != '"' || (size_t)(end - start) + 1U >= sizeof(uri))
            {
                result = 6;
                break;
            }
            size_t bytes = (size_t)(end - start) + 1U;
            memcpy(uri, start, bytes);
            uri[bytes] = '\0';

            stage = 4;
            if (strcmp(mode, "diagnostics-timeout") == 0)
                continue;
            if (strcmp(mode, "diagnostics-disconnect") == 0)
                break;
            const char *items =
                "[{\"message\":\"Unknown "
                "name\",\"severity\":2,\"source\":\"fixture\",\"code\":\"name\",\"range\":{\"start\":{"
                "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-empty") == 0)
                items = "[]";
            if (strcmp(mode, "diagnostics-invalid") == 0)
                items = "[{\"message\":\"missing range\"}]";
            if (strcmp(mode, "diagnostics-literal") == 0)
                items = "[{\"message\":\"<script>literal</"
                        "script>\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-outside") == 0 || strcmp(mode, "diagnostics-surrogate") == 0)
                items = "[{\"message\":\"invalid "
                        "endpoint\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":3}}}]";
            if (strcmp(mode, "diagnostics-related") == 0)
                items = "[{\"message\":\"name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                        "\"line\":0,\"character\":2}},\"relatedInformation\":[{\"message\":\"declared "
                        "here\",\"location\":{\"uri\":\"file:///"
                        "other.c\",\"range\":{\"start\":{\"line\":3,\"character\":0},\"end\":{\"line\":3,"
                        "\"character\":1}}}}]}]";
            if (strcmp(mode, "diagnostics-choices") == 0)
                items = "[{\"message\":\"First\",\"severity\":1,\"range\":{\"start\":{\"line\":0,"
                        "\"character\":0},\"end\":{\"line\":0,\"character\":1}}},{\"message\":\"Second\","
                        "\"severity\":4,\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":"
                        "0,\"character\":2}}}]";
            if (strcmp(mode, "diagnostics-wrong-uri") == 0 || strcmp(mode, "diagnostics-stale") == 0)
            {
                int n = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                    "publishDiagnostics\",\"params\":{\"uri\":%s,\"version\":%d,\"diagnostics\":[]}}",
                    strcmp(mode, "diagnostics-wrong-uri") == 0 ? "\"file:///other.c\"" : uri,
                    strcmp(mode, "diagnostics-stale") == 0 ? 0 : 1);
                if (n <= 0 || (size_t)n >= sizeof(response) || printf("Content-Length: %d\r\n\r\n", n) < 0 ||
                    fwrite(response, 1U, (size_t)n, stdout) != (size_t)n || fflush(stdout) != 0)
                {
                    result = 7;
                    break;
                }
            }
            char related_items[6144];
            if (strncmp(mode, "diagnostics-links-", 18U) == 0)
            {
                if (!DiagnosticRelatedFixtureItems(mode, uri, related_items, sizeof(related_items)))
                {
                    result = 8;
                    break;
                }
                items = related_items;
            }
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                         "publishDiagnostics\",\"params\":{\"uri\":%s,%s\"diagnostics\":%s}}",
                         uri, strcmp(mode, "diagnostics-unversioned") == 0 ? "" : "\"version\":1,", items);
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if (stage != 4)
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
            if (strcmp(mode, "diagnostics-shutdown") == 0)
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
