/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/rename_server_fixture.inc
 * PURPOSE: Check native rename ordering with an independent framed stdio peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Deliberately parse only fixture messages here. Keeping this peer independent
 * of the production reader catches request ordering and field mistakes. */
static int RenameFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[16384], uri[8192] = {0};
    int stage = 0, result = 0;
    int prepare = strncmp(mode, "rename-prepare", 14U) == 0;
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
            if (stage != 0 || strstr(body, "\"prepareSupport\":true") == NULL ||
                strstr(body, "\"honorsChangeAnnotations\":true") == NULL ||
                strstr(body, "\"resourceOperations\":[]") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            const char *provider = prepare ? "{\"prepareProvider\":true}" : "true";
            const char *encoding = strcmp(mode, "rename-encoding") == 0 ? "utf-8" : "utf-16";
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{\"renameProvider\":%"
                         "s,\"textDocumentSync\":{\"openClose\":true},\"positionEncoding\":\"%s\"}}}",
                         id, provider, encoding);
            stage = 1;
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
            if (stage != 2 || strstr(body, "\"version\":1") == NULL || strstr(body, "\"text\":") == NULL)
            {
                result = 5;
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
        else if (strstr(body, "\"method\":\"textDocument/prepareRename\"") != NULL)
        {
            if (stage != 3 || !prepare || strstr(body, "\"newName\":") != NULL ||
                strstr(body, "\"position\":") == NULL)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "rename-prepare-timeout") == 0)
                continue;
            const char *payload =
                "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}";
            if (strcmp(mode, "rename-prepare-null") == 0)
                payload = "null";
            if (strcmp(mode, "rename-prepare-default") == 0)
                payload = "{\"defaultBehavior\":true}";
            if (strcmp(mode, "rename-prepare-invalid") == 0)
                payload = "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                          "\"character\":3}}}";
            if (strcmp(mode, "rename-prepare-placeholder") == 0)
                payload = "{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                          "\"character\":3}},\"placeholder\":\"abc\"}";
            written = snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%s}",
                               id, payload);
        }
        else if (strstr(body, "\"method\":\"textDocument/rename\"") != NULL)
        {
            if (stage != (prepare ? 4 : 3) || strstr(body, "\"newName\":\"total\"") == NULL ||
                strstr(body, "\"position\":") == NULL)
            {
                result = 5;
                break;
            }
            if (strcmp(mode, "rename-unicode") == 0 && strstr(body, "\"character\":3") == NULL)
            {
                result = 6;
                break;
            }
            stage = 5;
            if (strcmp(mode, "rename-timeout") == 0)
                continue;
            if (strcmp(mode, "rename-disconnect") == 0)
                break;
            if (strcmp(mode, "rename-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else if (strcmp(mode, "rename-empty") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else if (strcmp(mode, "rename-invalid") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"changes\":7}}", id);
            else if (strcmp(mode, "rename-resource") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"documentChanges\":[{"
                                   "\"kind\":\"create\",\"uri\":%s}]}}",
                                   id, uri);
            else
            {
                const char *version = strcmp(mode, "rename-stale") == 0          ? "2"
                                      : strcmp(mode, "rename-null-version") == 0 ? "null"
                                                                                 : "1";
                const char *end = strcmp(mode, "rename-outside") == 0 ? "99" : "3";
                const char *annotation =
                    strcmp(mode, "rename-annotation") == 0 ? ",\"annotationId\":\"reason\"" : "";
                const char *annotations = strcmp(mode, "rename-annotation") == 0
                                              ? ",\"changeAnnotations\":{\"reason\":{\"label\":\"Rename "
                                                "references\",\"needsConfirmation\":true}}"
                                              : "";
                const char *target =
                    strcmp(mode, "rename-external") == 0 ? "\"file:///workspace/other.c\"" : uri;
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"documentChanges\":[{\"textDocument\":{"
                    "\"uri\":%s,\"version\":%s},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0}"
                    ",\"end\":{\"line\":0,\"character\":%s}},\"newText\":\"total\"%s}]}]%s}}",
                    id, target, version, end, annotation, annotations);
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if ((stage != 4 && stage != 5) || strstr(body, uri) == NULL)
            {
                result = 5;
                break;
            }
            stage = 6;
        }
        else if (strstr(body, "\"method\":\"shutdown\"") != NULL)
        {
            if (stage != 6)
            {
                result = 5;
                break;
            }
            stage = 7;
            if (strcmp(mode, "rename-shutdown") == 0)
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
            if (stage != 7)
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
