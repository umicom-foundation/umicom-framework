/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/symbol_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native document-symbol requests and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
static int SymbolFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], current_uri[4096] = {0};
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
            if (stage != 0 || strstr(body, "\"hierarchicalDocumentSymbolSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "symbols-encoding") == 0
                                   ? "\"documentSymbolProvider\":true,\"textDocumentSync\":2,"
                                     "\"positionEncoding\":\"utf-8\""
                                   : "\"documentSymbolProvider\":true,\"textDocumentSync\":{"
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
            if (strcmp(mode, "symbols-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
            {
                result = 6;
                break;
            }
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
            stage = 3;
        }
        else if (strstr(body, "\"method\":\"textDocument/documentSymbol\"") != NULL)
        {
            if (stage != 3)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "symbols-timeout") == 0)
                continue;
            if (strcmp(mode, "symbols-disconnect") == 0)
                break;
            /* Document outline requests contain no caret or completion context. */
            if (strstr(body, "\"position\":") != NULL || strstr(body, "\"options\":") != NULL ||
                strstr(body, "\"context\":") != NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "symbols-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else if (strcmp(mode, "symbols-invalid") == 0)
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"isIncomplete\":false}}", id);
            else if (strcmp(mode, "symbols-empty") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else
            {
                const char *range =
                    "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}";
                const char *selection = range;
                if (strcmp(mode, "symbols-range") == 0)
                    range = "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":9,\"character\":0}}";
                if (strcmp(mode, "symbols-surrogate") == 0)
                    selection =
                        "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}}";
                if (strcmp(mode, "symbols-flat") == 0)
                    written =
                        snprintf(response, sizeof(response),
                                 "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"name\":\"pu\",\"kind\":12,"
                                 "\"containerName\":\"file\",\"location\":{\"uri\":%s,\"range\":%s}}]}",
                                 id, current_uri, range);
                else if (strcmp(mode, "symbols-hierarchy") == 0)
                    written = snprintf(response, sizeof(response),
                                       "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"name\":\"outer\","
                                       "\"kind\":5,\"range\":%s,\"selectionRange\":%s,\"children\":[{"
                                       "\"name\":\"pu\",\"kind\":12,\"range\":%s,\"selectionRange\":%s}]}]}",
                                       id, range, selection, range, selection);
                else
                    written =
                        snprintf(response, sizeof(response),
                                 "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"name\":\"pu\",\"kind\":%d,"
                                 "\"range\":%s,\"selectionRange\":%s}]}",
                                 id, strcmp(mode, "symbols-unknown-kind") == 0 ? 500 : 12, range, selection);
            }
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
            if (strcmp(mode, "symbols-shutdown") == 0)
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
