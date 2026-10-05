/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/code_action_server_fixture.inc
 * PURPOSE: Provide an independent stdio peer for native code-action requests and cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This small peer checks wire ordering independently of Framework's parser.
 * It is intentionally limited to the explicit request shapes in these tests. */
/* The independent peer now publishes diagnostic-owned fix data before accepting an optional same-session action request.
 * The previous implementation is retained for engineering review. */
#if 0
static int CodeActionFixture(const char *mode)
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
            if (stage != 0 || strstr(body, "\"disabledSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "actions-encoding") == 0
                                   ? "\"codeActionProvider\":true,\"textDocumentSync\":2,"
                                     "\"positionEncoding\":\"utf-8\""
                                   : "\"codeActionProvider\":true,\"textDocumentSync\":{"
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
            if (strcmp(mode, "actions-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
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
        else if (strstr(body, "\"method\":\"textDocument/codeAction\"") != NULL)
        {
            if (stage != 3)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "actions-timeout") == 0)
                continue;
            if (strcmp(mode, "actions-disconnect") == 0)
                break;
            /* The selected range is explicit and no diagnostics from another
             * connection are fabricated. Commands are never sent to this peer. */
            if (strstr(body, "\"range\":") == NULL ||
                strstr(body, "\"diagnostics\":[],\"triggerKind\":1") == NULL ||
                strstr(body, "\"position\":") != NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-unicode") == 0 &&
                strstr(body, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                             "\"character\":2}}") == NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-multiline") == 0 &&
                strstr(body, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":1,"
                             "\"character\":1}}") == NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else if (strcmp(mode, "actions-invalid") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":7}]}", id);
            else if (strcmp(mode, "actions-empty") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else if (strcmp(mode, "actions-command") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Server "
                                   "command\",\"command\":\"server.run\",\"arguments\":[1]}]}",
                                   id);
            else if (strcmp(mode, "actions-deferred") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Resolve "
                                   "later\",\"data\":{\"token\":7}}]}",
                                   id);
            else if (strcmp(mode, "actions-resource") == 0)
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Create "
                             "helper\",\"edit\":{\"documentChanges\":[{\"kind\":\"create\",\"uri\":%s}]}}]}",
                             id, uri);
            else
            {
                const char *disabled = strcmp(mode, "actions-disabled") == 0
                                           ? ",\"disabled\":{\"reason\":\"Select an expression\"}"
                                           : "";
                const char *command = strcmp(mode, "actions-edit-command") == 0
                                          ? ",\"command\":{\"title\":\"Finish\",\"command\":\"server.run\"}"
                                          : "";
                const char *annotation =
                    strcmp(mode, "actions-annotation") == 0 ? ",\"annotationId\":\"reason\"" : "";
                const char *annotations = strcmp(mode, "actions-annotation") == 0
                                              ? ",\"changeAnnotations\":{\"reason\":{\"label\":\"Review this "
                                                "refactor\",\"needsConfirmation\":true}}"
                                              : "";
                const char *title =
                    strcmp(mode, "actions-literal") == 0 ? "<script>literal</script>" : "Extract variable";
                const char *target =
                    strcmp(mode, "actions-external") == 0 ? "\"file:///workspace/other.c\"" : uri;
                const char *end = strcmp(mode, "actions-unicode") == 0 ? "2" : "3";
                const char *extra =
                    strcmp(mode, "actions-choices") == 0
                        ? ",{\"title\":\"Unavailable alternative\",\"disabled\":{\"reason\":\"Not supported "
                          "for this range\"}}"
                        : "";
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"%s\",\"kind\":\"refactor."
                    "extract\",\"isPreferred\":true%s%s,\"edit\":{\"documentChanges\":[{\"textDocument\":{"
                    "\"uri\":%s,\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                    "\"end\":{\"line\":0,\"character\":%s}},\"newText\":\"total\"%s}]}]%s}}%s]}",
                    id, title, disabled, command, target, end, annotation, annotations, extra);
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
            if (strcmp(mode, "actions-shutdown") == 0)
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
static int CodeActionFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[16384], uri[8192] = {0};
    int stage = 0, result = 0;
    int context = strncmp(mode, "actions-context-", 16U) == 0;
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
            if (stage != 0 || strstr(body, "\"disabledSupport\":true") == NULL ||
                strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            stage = 1;
            const char *caps = strcmp(mode, "actions-encoding") == 0
                                   ? "\"codeActionProvider\":true,\"textDocumentSync\":2,"
                                     "\"positionEncoding\":\"utf-8\""
                                   : "\"codeActionProvider\":true,\"textDocumentSync\":{"
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
            if (strcmp(mode, "actions-unicode") == 0 && strstr(body, "\xf0\x9f\x98\x80") == NULL)
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
            if (context)
            {
                if (strcmp(mode, "actions-context-missing") == 0)
                    continue;
                const char *diagnostics =
                    strcmp(mode, "actions-context-empty") == 0 ? "[]"
                    : strcmp(mode, "actions-context-invalid") == 0
                        ? "[{\"message\":\"missing range\"}]"
                        : "[{\"message\":\"name\",\"data\":{\"opaque\":\"fixture-session\"},\"range\":{"
                          "\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}}]";
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                             "publishDiagnostics\",\"params\":{\"uri\":%s,\"version\":1,\"diagnostics\":%s}}",
                             uri, diagnostics);
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/codeAction\"") != NULL)
        {
            if (stage != 3)
            {
                result = 5;
                break;
            }
            stage = 4;
            if (strcmp(mode, "actions-timeout") == 0)
                continue;
            if (strcmp(mode, "actions-disconnect") == 0)
                break;
            /* The selected range is explicit and no diagnostics from another
             * connection are fabricated. Commands are never sent to this peer. */
            if (strstr(body, "\"range\":") == NULL ||
                (context && strcmp(mode, "actions-context-empty") != 0
                     ? strstr(body, "\"opaque\":\"fixture-session\"") == NULL
                     : strstr(body, "\"diagnostics\":[],\"triggerKind\":1") == NULL) ||
                strstr(body, "\"position\":") != NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-unicode") == 0 &&
                strstr(body, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                             "\"character\":2}}") == NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-multiline") == 0 &&
                strstr(body, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":1,"
                             "\"character\":1}}") == NULL)
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "actions-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":\"refused\"}}",
                    id);
            else if (strcmp(mode, "actions-invalid") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":7}]}", id);
            else if (strcmp(mode, "actions-empty") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else if (strcmp(mode, "actions-command") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Server "
                                   "command\",\"command\":\"server.run\",\"arguments\":[1]}]}",
                                   id);
            else if (strcmp(mode, "actions-deferred") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Resolve "
                                   "later\",\"data\":{\"token\":7}}]}",
                                   id);
            else if (strcmp(mode, "actions-resource") == 0)
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Create "
                             "helper\",\"edit\":{\"documentChanges\":[{\"kind\":\"create\",\"uri\":%s}]}}]}",
                             id, uri);
            else
            {
                const char *disabled = strcmp(mode, "actions-disabled") == 0
                                           ? ",\"disabled\":{\"reason\":\"Select an expression\"}"
                                           : "";
                const char *command = strcmp(mode, "actions-edit-command") == 0
                                          ? ",\"command\":{\"title\":\"Finish\",\"command\":\"server.run\"}"
                                          : "";
                const char *annotation =
                    strcmp(mode, "actions-annotation") == 0 ? ",\"annotationId\":\"reason\"" : "";
                const char *annotations = strcmp(mode, "actions-annotation") == 0
                                              ? ",\"changeAnnotations\":{\"reason\":{\"label\":\"Review this "
                                                "refactor\",\"needsConfirmation\":true}}"
                                              : "";
                const char *title =
                    strcmp(mode, "actions-literal") == 0 ? "<script>literal</script>" : "Extract variable";
                const char *target =
                    strcmp(mode, "actions-external") == 0 ? "\"file:///workspace/other.c\"" : uri;
                const char *end = strcmp(mode, "actions-unicode") == 0 ? "2" : "3";
                const char *extra =
                    strcmp(mode, "actions-choices") == 0
                        ? ",{\"title\":\"Unavailable alternative\",\"disabled\":{\"reason\":\"Not supported "
                          "for this range\"}}"
                        : "";
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"%s\",\"kind\":\"refactor."
                    "extract\",\"isPreferred\":true%s%s,\"edit\":{\"documentChanges\":[{\"textDocument\":{"
                    "\"uri\":%s,\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
                    "\"end\":{\"line\":0,\"character\":%s}},\"newText\":\"total\"%s}]}]%s}}%s]}",
                    id, title, disabled, command, target, end, annotation, annotations, extra);
            }
        }

        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            if (stage != 4 && !(context && stage == 3))
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
            if (strcmp(mode, "actions-shutdown") == 0)
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
