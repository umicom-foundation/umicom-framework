/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/action_resolution_fixture.inc
 * PURPOSE: Independently check complete action resolution before source closure and return reviewed workspace proposals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static int ActionResolutionFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[16384], proposal[6144], action[2048];
    unsigned resolved = 0U;
    unsigned stage = 0U, opened = 0U, closed = 0U;
    unsigned char remaining[3] = {0};
    int queried = 0, result = 0, pulled = 0;
    int diagnostics = strcmp(mode, "actions-resolve-empty-context") != 0;
    const char *family = strcmp(mode, "actions-resolve-quickfix") == 0   ? "quickfix"
                         : strcmp(mode, "actions-resolve-refactor") == 0 ? "refactor"
                         : strcmp(mode, "actions-resolve-fix-all") == 0  ? "source.fixAll"
                                                                         : "source.organizeImports";
    const char *uris[] = {"file:///workspace/main.c", "file:///workspace/second.c",
                          "file:///workspace/third.c"};
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
                unsigned long bytes = strtoul(line + 16U, &end, 10);
                if (end == line + 16U || (*end != '\r' && *end != '\n') || bytes >= 65536UL)
                {
                    result = 4;
                    goto done;
                }
                length = (size_t)bytes;
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
            if (stage != 0U || strstr(body, "\"positionEncodings\":[\"utf-16\"]") == NULL)
            {
                result = 5;
                break;
            }
            if (strstr(body, "\"resolveSupport\":{\"properties\":[\"edit\"]}") == NULL ||
                strstr(body, "\"dataSupport\":true") == NULL ||
                strstr(body, "\"relatedDocumentSupport\":false") == NULL)
            {
                result = 19;
                break;
            }
            written = snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                "\"textDocumentSync\":{\"openClose\":true,\"change\":1},\"codeActionProvider\":{"
                "\"resolveProvider\":true},"
                "\"diagnosticProvider\":{\"interFileDependencies\":true,\"workspaceDiagnostics\":false}}}}",
                id);
            stage = 1U;
        }
        else if (strstr(body, "\"method\":\"initialized\"") != NULL)
        {
            if (stage != 1U)
            {
                result = 6;
                break;
            }
            stage = 2U;
        }
        else if (strstr(body, "\"method\":\"textDocument/didOpen\"") != NULL)
        {
            const char *texts[] = {"source main", "source second", "source third"};
            if (stage != 2U || opened >= 3U || queried || strstr(body, uris[opened]) == NULL ||
                strstr(body, texts[opened]) == NULL || strstr(body, "\"version\":1") == NULL)
            {
                result = 7;
                break;
            }
            if (opened == 1U && strcmp(mode, "actions-resolve-unicode") == 0 &&
                strstr(body, "caf\xc3\xa9") == NULL)
            {
                result = 8;
                break;
            }
            remaining[opened] = 1U;
            ++opened;
            /* The separate publication peer remains available. Here every
             * source must be opened before an explicit diagnostic request. */
        }
        else if (strstr(body, "\"method\":\"textDocument/diagnostic\"") != NULL)
        {
            if (opened != 3U || pulled || queried || strstr(body, uris[0]) == NULL ||
                strstr(body, "previousResultId") != NULL)
            {
                result = 18;
                break;
            }
            pulled = 1;
            const char *items =
                diagnostics ? "[{\"message\":\"Use a clearer "
                              "name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                              "\"character\":6}},\"data\":{\"opaque\":\"same source session\"}}]"
                            : "[]";
            if (strcmp(mode, "actions-resolve-diagnostic-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32802,\"message\":\"later\"}}",
                    id);
            else if (strcmp(mode, "actions-resolve-unchanged") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"unchanged\","
                                   "\"resultId\":\"old\"}}",
                                   id);
            else
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"full\",\"items\":%s%s}}", id,
                    items,
                    strcmp(mode, "actions-resolve-related-report") == 0
                        ? ",\"relatedDocuments\":{\"file:///other.c\":{\"kind\":\"full\",\"items\":[]}}"
                        : "");
        }
        else if (strstr(body, "\"method\":\"textDocument/codeAction\"") != NULL)
        {
            if (opened != 3U || !pulled || queried || strstr(body, uris[0]) == NULL ||
                (diagnostics && strstr(body, "same source session") == NULL) ||
                (!diagnostics && strstr(body, "\"diagnostics\":[]") == NULL))
            {
                result = 10;
                break;
            }
            char expected_only[96];
            (void)snprintf(expected_only, sizeof(expected_only), "\"only\":[\"%s\"]", family);
            if (strstr(body, expected_only) == NULL)
            {
                result = 20;
                break;
            }
            queried = 1;
            if (strcmp(mode, "actions-resolve-query-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"refused\"}}", id);
            else
            {
                const char *edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                   "\"character\":6}},\"newText\":\"renamed\"}]";
                if (strcmp(mode, "actions-resolve-outside") == 0)
                    edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":99}},\"newText\":\"renamed\"}]";
                int size;
                if (strcmp(mode, "actions-resolve-all") == 0)
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s,\"%s\":%s,\"%s\":%s}}",
                                 uris[0], edit, uris[1], edit, uris[2], edit);
                else if (strcmp(mode, "actions-resolve-stale") == 0)
                    size = snprintf(proposal, sizeof(proposal),
                                    "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":2},"
                                    "\"edits\":%s}]}",
                                    uris[1], edit);
                else
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}",
                                 strcmp(mode, "actions-resolve-unknown") == 0 ? "file:///workspace/unknown.c"
                                                                              : uris[1],
                                 edit);
                if (size < 0 || (size_t)size >= sizeof(proposal))
                {
                    result = 11;
                    break;
                }
                const char *metadata = strcmp(mode, "actions-resolve-disabled") == 0
                                           ? ",\"disabled\":{\"reason\":\"unavailable\"}"
                                       : strcmp(mode, "actions-resolve-command") == 0
                                           ? ",\"command\":{\"title\":\"Run\",\"command\":\"never run\"}"
                                           : "";
                const char *data =
                    strcmp(mode, "actions-resolve-no-data") == 0
                        ? ""
                        : ",\"data\":{\"session\":\"same source session\",\"opaque\":[1,true]}";
                int action_size = snprintf(action, sizeof(action),
                                           "{\"title\":\"Review complete fix\",\"kind\":\"%s\"%s%s}", family,
                                           data, metadata);
                if (action_size <= 0 || (size_t)action_size >= sizeof(action))
                {
                    result = 21;
                    break;
                }
                if (strcmp(mode, "actions-resolve-complete") == 0)
                    written = snprintf(response, sizeof(response),
                                       "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[%.*s,\"edit\":%s}]}", id,
                                       action_size - 1, action, proposal);
                else if (strcmp(mode, "actions-resolve-choices") == 0)
                    written = snprintf(
                        response, sizeof(response),
                        "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Unavailable "
                        "choice\",\"kind\":\"%s\",\"disabled\":{\"reason\":\"choose another\"}},%s]}",
                        id, family, action);
                else
                    written =
                        snprintf(response, sizeof(response),
                                 "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Other\",\"kind\":"
                                 "\"source.other\",\"data\":9},%s,{\"title\":\"Unclassified\",\"edit\":{}}]}",
                                 id, action);
            }
        }
        else if (strstr(body, "\"method\":\"codeAction/resolve\"") != NULL)
        {
            if (!queried || opened != 3U || closed != 0U || resolved != 0U || strstr(body, action) == NULL)
            {
                result = 22;
                break;
            }
            ++resolved;
            if (strcmp(mode, "actions-resolve-timeout") == 0)
                continue;
            if (strcmp(mode, "actions-resolve-disconnect") == 0)
                break;
            if (strcmp(mode, "actions-resolve-error") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":"
                                   "\"resolve refused\"}}",
                                   id);
            else if (strcmp(mode, "actions-resolve-missing-edit") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%s}", id, action);
            else if (strcmp(mode, "actions-resolve-malformed") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else
            {
                if (strcmp(mode, "actions-resolve-changed-title") == 0)
                    action[10] = 'X';
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%.*s,\"edit\":%s%s}}", id,
                                   (int)strlen(action) - 1, action, proposal,
                                   strcmp(mode, "actions-resolve-added-command") == 0
                                       ? ",\"command\":{\"title\":\"Run\",\"command\":\"never\"}"
                                       : "");
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            int match = -1;
            for (unsigned i = 0U; i < 3U; ++i)
                if (strstr(body, uris[i]) != NULL)
                    match = (int)i;
            if (!pulled || match < 0 || !remaining[match])
            {
                result = 12;
                break;
            }
            remaining[match] = 0U;
            ++closed;
        }
        else if (strstr(body, "\"method\":\"shutdown\"") != NULL)
        {
            if (opened != 3U || closed != 3U || remaining[0] || remaining[1] || remaining[2])
            {
                result = 13;
                break;
            }
            unsigned expected_resolves = queried && strcmp(mode, "actions-resolve-query-error") != 0 &&
                                                 strcmp(mode, "actions-resolve-disabled") != 0 &&
                                                 strcmp(mode, "actions-resolve-command") != 0 &&
                                                 strcmp(mode, "actions-resolve-complete") != 0
                                             ? 1U
                                             : 0U;
            if (resolved != expected_resolves)
            {
                result = 23;
                break;
            }
            stage = 3U;
            if (strcmp(mode, "actions-resolve-shutdown-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"shutdown\"}}", id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
        }
        else if (strstr(body, "\"method\":\"exit\"") != NULL)
        {
            if (stage != 3U)
                result = 14;
            break;
        }
        else
        {
            result = 15;
            break;
        }
        if (written != 0)
        {
            if (written < 0 || (size_t)written >= sizeof(response))
            {
                result = 16;
                break;
            }
            if (printf("Content-Length: %d\r\n\r\n%s", written, response) < 0 || fflush(stdout) != 0)
            {
                result = 17;
                break;
            }
        }
    }
done:
    free(body);
    return result;
}
