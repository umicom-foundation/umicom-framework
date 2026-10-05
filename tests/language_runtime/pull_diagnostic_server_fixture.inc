/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/pull_diagnostic_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native explicit diagnostic requests and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
static int PullDiagnosticFixture(const char *mode)
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
            if (stage != 0 || strstr(body, "\"relatedDocumentSupport\":false") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps =
                strcmp(mode, "pull-diagnostics-encoding") == 0
                    ? "\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\""
                : strcmp(mode, "pull-diagnostics-unsupported") == 0
                    ? "\"textDocumentSync\":2"
                    : "\"textDocumentSync\":2,\"diagnosticProvider\":{\"interFileDependencies\":true,"
                      "\"workspaceDiagnostics\":false,\"identifier\":\"checker\"}";
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
            if (strcmp(mode, "pull-diagnostics-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
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

            stage = 3;
        }
        else if (strstr(body, "\"method\":\"textDocument/diagnostic\"") != NULL)
        {
            if (stage != 3 || strstr(body, uri) == NULL ||
                strstr(body, "\"identifier\":\"checker\"") == NULL ||
                strstr(body, "previousResultId") != NULL)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "pull-diagnostics-timeout") == 0)
                continue;
            const char *items =
                "[{\"message\":\"Unknown "
                "name\",\"severity\":2,\"source\":\"fixture\",\"code\":\"name\",\"range\":{\"start\":{"
                "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}}]";
            if (strcmp(mode, "pull-diagnostics-empty") == 0)
                items = "[]";
            if (strcmp(mode, "pull-diagnostics-invalid") == 0)
                items = "[{\"message\":\"missing range\"}]";
            if (strcmp(mode, "pull-diagnostics-literal") == 0)
                items = "[{\"message\":\"<script>literal</"
                        "script>\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":2}}}]";
            if (strcmp(mode, "pull-diagnostics-outside") == 0 ||
                strcmp(mode, "pull-diagnostics-surrogate") == 0)
                items = "[{\"message\":\"invalid "
                        "endpoint\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                        "\"character\":3}}}]";
            if (strcmp(mode, "pull-diagnostics-related") == 0)
                items = "[{\"message\":\"name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
                        "\"line\":0,\"character\":2}},\"relatedInformation\":[{\"message\":\"declared "
                        "here\",\"location\":{\"uri\":\"file:///"
                        "other.c\",\"range\":{\"start\":{\"line\":3,\"character\":0},\"end\":{\"line\":3,"
                        "\"character\":1}}}}]}]";
            if (strcmp(mode, "pull-diagnostics-choices") == 0)
                items = "[{\"message\":\"First\",\"severity\":1,\"range\":{\"start\":{\"line\":0,"
                        "\"character\":0},\"end\":{\"line\":0,\"character\":1}}},{\"message\":\"Second\","
                        "\"severity\":4,\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{\"line\":"
                        "0,\"character\":2}}}]";

            if (strcmp(mode, "pull-diagnostics-unchanged") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"unchanged\","
                                   "\"resultId\":\"old\"}}",
                                   id);
            else if (strcmp(mode, "pull-diagnostics-query-error") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32802,\"message\":"
                                   "\"retry later\"}}",
                                   id);
            else
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"full\",\"resultId\":\"report-"
                    "key\",\"items\":%s%s}}",
                    id, items,
                    strcmp(mode, "pull-diagnostics-related-report") == 0
                        ? ",\"relatedDocuments\":{\"file:///other.c\":{\"kind\":\"full\",\"items\":[]}}"
                        : "");
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
            if (strcmp(mode, "pull-diagnostics-shutdown") == 0)
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
