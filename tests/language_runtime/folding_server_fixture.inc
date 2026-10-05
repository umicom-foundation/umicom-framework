/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/folding_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native complete-line folding requests and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
static int FoldingRangeFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], current_uri[4096] = "\"file:///workspace/main.c\"";
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
            if (stage != 0 ||
                strstr(body, "\"foldingRange\":{\"dynamicRegistration\":false,\"rangeLimit\":4096,"
                             "\"lineFoldingOnly\":true}") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps =
                strcmp(mode, "foldings-encoding") == 0
                    ? "\"foldingRangeProvider\":true,\"textDocumentSync\":2,\"positionEncoding\":\"utf-8\""
                    : "\"foldingRangeProvider\":true,\"textDocumentSync\":{\"openClose\":true,\"change\":"
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
            if (strcmp(mode, "foldings-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
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
        else if (strstr(body, "\"method\":\"textDocument/foldingRange\"") != NULL)
        {
            if (stage != 3)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "foldings-timeout") == 0)
                continue;
            if (strcmp(mode, "foldings-disconnect") == 0)
                break;
            if (strstr(body, current_uri) == NULL || strstr(body, "\"position\":") != NULL ||
                strstr(body, "\"positions\":") != NULL || strstr(body, "includeDeclaration") != NULL)
            {
                result = 6;
                break;
            }
            const char *ranges = "[{\"startLine\":0,\"endLine\":2},{\"startLine\":1,\"endLine\":2}]";
            if (strcmp(mode, "foldings-single") == 0)
                ranges = "[{\"startLine\":1,\"endLine\":1}]";
            if (strcmp(mode, "foldings-outside") == 0)
                ranges = "[{\"startLine\":0,\"endLine\":8}]";
            if (strcmp(mode, "foldings-invalid") == 0)
                ranges = "[{\"startLine\":0,\"endLine\":2},{}]";
            if (strcmp(mode, "foldings-empty-array") == 0)
                ranges = "[]";
            if (strcmp(mode, "foldings-empty") == 0)
                ranges = "null";
            if (strcmp(mode, "foldings-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%s}", id, ranges);
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
            if (strcmp(mode, "foldings-shutdown") == 0)
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
