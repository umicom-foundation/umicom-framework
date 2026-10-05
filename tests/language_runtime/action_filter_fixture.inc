/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/action_filter_fixture.inc
 * PURPOSE: Require a selected action family alongside fresh diagnostics and return mixed complete proposals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static int ActionFilterFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], proposal[6144];
    unsigned stage = 0U, opened = 0U, closed = 0U;
    unsigned char remaining[3] = {0};
    int queried = 0, result = 0, pulled = 0;
    int diagnostics = strcmp(mode, "actions-filter-empty-context") != 0;
    const char *family = strcmp(mode, "actions-filter-quickfix") == 0   ? "quickfix"
                         : strcmp(mode, "actions-filter-refactor") == 0 ? "refactor"
                         : strcmp(mode, "actions-filter-fix-all") == 0  ? "source.fixAll"
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
            /* The peer must see the capabilities needed to retain diagnostic
             * data and must never be asked for related-document reports. */
            if (strstr(body, "\"relatedDocumentSupport\":false") == NULL ||
                strstr(body, "\"dataSupport\":true") == NULL)
            {
                result = 19;
                break;
            }
            written = snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                "\"textDocumentSync\":{\"openClose\":true,\"change\":1},\"codeActionProvider\":true,"
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
            if (opened == 1U && strcmp(mode, "actions-filter-unicode") == 0 &&
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
            if (strcmp(mode, "actions-filter-diagnostic-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32802,\"message\":\"later\"}}",
                    id);
            else if (strcmp(mode, "actions-filter-unchanged") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"unchanged\","
                                   "\"resultId\":\"old\"}}",
                                   id);
            else
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"kind\":\"full\",\"items\":%s%s}}", id,
                    items,
                    strcmp(mode, "actions-filter-related-report") == 0
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
            if (strcmp(mode, "actions-filter-query-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"refused\"}}", id);
            else
            {
                const char *edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                   "\"character\":6}},\"newText\":\"renamed\"}]";
                if (strcmp(mode, "actions-filter-outside") == 0)
                    edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":99}},\"newText\":\"renamed\"}]";
                int size;
                if (strcmp(mode, "actions-filter-all") == 0)
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s,\"%s\":%s,\"%s\":%s}}",
                                 uris[0], edit, uris[1], edit, uris[2], edit);
                else if (strcmp(mode, "actions-filter-stale") == 0)
                    size = snprintf(proposal, sizeof(proposal),
                                    "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":2},"
                                    "\"edits\":%s}]}",
                                    uris[1], edit);
                else
                    size = snprintf(
                        proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}",
                        strcmp(mode, "actions-filter-unknown") == 0 ? "file:///workspace/unknown.c" : uris[1],
                        edit);
                if (size < 0 || (size_t)size >= sizeof(proposal))
                {
                    result = 11;
                    break;
                }
                const char *metadata = strcmp(mode, "actions-filter-disabled") == 0
                                           ? ",\"disabled\":{\"reason\":\"unavailable\"}"
                                       : strcmp(mode, "actions-filter-command") == 0
                                           ? ",\"command\":{\"title\":\"Run\",\"command\":\"never run\"}"
                                           : "";
                const char *kind =
                    strcmp(mode, "actions-filter-descendants") == 0 ? "source.organizeImports.cpp" : family;
                /* Deliberately return unrelated rows as well. A request filter
                 * is advisory to the server, so the client must filter locally
                 * without losing a matching action's full workspace edit. */
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Other\",\"kind\":\"source."
                    "other\",\"edit\":{}},{\"title\":\"Review complete "
                    "fix\",\"kind\":\"%s\",\"edit\":%s%s},{\"title\":\"Unclassified\",\"edit\":{}}]}",
                    id, kind, proposal, metadata);
                if (strcmp(mode, "actions-filter-choices") == 0)
                    written = snprintf(response, sizeof(response),
                                       "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Other\","
                                       "\"kind\":\"source.other\",\"edit\":{}},{\"title\":\"Unavailable "
                                       "choice\",\"kind\":\"%s\",\"disabled\":{\"reason\":\"choose "
                                       "another\"},\"edit\":{}},{\"title\":\"Complete "
                                       "choice\",\"kind\":\"%s\",\"edit\":%s}]}",
                                       id, kind, kind, proposal);
                if (strcmp(mode, "actions-filter-ignored") == 0 ||
                    strcmp(mode, "actions-filter-prefix-boundary") == 0)
                    written = snprintf(response, sizeof(response),
                                       "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Other\","
                                       "\"kind\":\"%s\",\"edit\":%s}]}",
                                       id,
                                       strcmp(mode, "actions-filter-prefix-boundary") == 0
                                           ? "source.organizeImportsOther"
                                           : "quickfix",
                                       proposal);
                if (strcmp(mode, "actions-filter-malformed-unrelated") == 0)
                    written = snprintf(
                        response, sizeof(response),
                        "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Complete "
                        "choice\",\"kind\":\"%s\",\"edit\":%s},{\"title\":false,\"kind\":\"quickfix\"}]}",
                        id, kind, proposal);
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
            stage = 3U;
            if (strcmp(mode, "actions-filter-shutdown-error") == 0)
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
