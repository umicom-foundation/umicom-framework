/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/action_sources_fixture.inc
 * PURPOSE: Independently verify code actions receive multiple drafts and optional same-session diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This peer uses literal fixture messages rather than the production decoder,
 * so a shared serialization mistake cannot make both sides agree by accident. */
/* Add independent action selection while retaining the original peer for review. */
#if 0
static int ActionSourcesFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], proposal[6144];
    unsigned stage = 0U, opened = 0U, closed = 0U;
    unsigned char remaining[3] = {0};
    int queried = 0, result = 0;
    int diagnostics = strcmp(mode,"actions-sources-diagnostics")==0;
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
            written =
                snprintf(response, sizeof(response),
                         "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                         "\"textDocumentSync\":{\"openClose\":true,\"change\":1},\"codeActionProvider\":true}}}", id);
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
            if (opened == 1U && strcmp(mode, "actions-sources-unicode") == 0 &&
                strstr(body, "caf\xc3\xa9") == NULL)
            {
                result = 8;
                break;
            }
            remaining[opened] = 1U;
            ++opened;
            if(opened==3U && diagnostics)
                written=snprintf(response,sizeof(response),"{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\",\"params\":{\"uri\":\"%s\",\"version\":1,\"diagnostics\":[{\"message\":\"Use a clearer name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":6}},\"data\":{\"opaque\":\"same source session\"}}]}}",uris[0]);

        }
        else if (strstr(body, "\"method\":\"textDocument/codeAction\"") != NULL)
        {
            if (opened != 3U || queried || strstr(body, uris[0]) == NULL ||
                (diagnostics && strstr(body,"same source session")==NULL) ||
                (!diagnostics && strstr(body,"\"diagnostics\":[]")==NULL))
            {
                result = 10;
                break;
            }
            queried = 1;
            if (strcmp(mode, "actions-sources-query-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"refused\"}}", id);
            else
            {
                const char *edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                   "\"character\":6}},\"newText\":\"renamed\"}]";
                if (strcmp(mode, "actions-sources-outside") == 0)
                    edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":99}},\"newText\":\"renamed\"}]";
                int size;
                if (strcmp(mode, "actions-sources-all") == 0)
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s,\"%s\":%s,\"%s\":%s}}",
                                 uris[0], edit, uris[1], edit, uris[2], edit);
                else if (strcmp(mode, "actions-sources-stale") == 0)
                    size = snprintf(proposal, sizeof(proposal),
                                    "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":2},"
                                    "\"edits\":%s}]}",
                                    uris[1], edit);
                else
                    size = snprintf(
                        proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}",
                        strcmp(mode, "actions-sources-unknown") == 0 ? "file:///workspace/unknown.c" : uris[1],
                        edit);
                if (size < 0 || (size_t)size >= sizeof(proposal))
                {
                    result = 11;
                    break;
                }
                const char *metadata=strcmp(mode,"actions-sources-disabled")==0?",\"disabled\":{\"reason\":\"unavailable\"}":
                    strcmp(mode,"actions-sources-command")==0?",\"command\":{\"title\":\"Run\",\"command\":\"never run\"}":"";
                written=snprintf(response,sizeof(response),"{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Review complete fix\",\"edit\":%s%s}]}",id,proposal,metadata);
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            int match = -1;
            for (unsigned i = 0U; i < 3U; ++i)
                if (strstr(body, uris[i]) != NULL)
                    match = (int)i;
            if (!queried || match < 0 || !remaining[match])
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
            if (strcmp(mode, "actions-sources-shutdown-error") == 0)
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

#endif
static int ActionSourcesFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], proposal[6144];
    unsigned stage = 0U, opened = 0U, closed = 0U;
    unsigned char remaining[3] = {0};
    int queried = 0, result = 0;
    int diagnostics = strcmp(mode, "actions-sources-diagnostics") == 0;
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
            written = snprintf(
                response, sizeof(response),
                "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                "\"textDocumentSync\":{\"openClose\":true,\"change\":1},\"codeActionProvider\":true}}}",
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
            if (opened == 1U && strcmp(mode, "actions-sources-unicode") == 0 &&
                strstr(body, "caf\xc3\xa9") == NULL)
            {
                result = 8;
                break;
            }
            remaining[opened] = 1U;
            ++opened;
            if (opened == 3U && diagnostics)
                written =
                    snprintf(response, sizeof(response),
                             "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                             "publishDiagnostics\",\"params\":{\"uri\":\"%s\",\"version\":1,\"diagnostics\":["
                             "{\"message\":\"Use a clearer "
                             "name\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                             "\"character\":6}},\"data\":{\"opaque\":\"same source session\"}}]}}",
                             uris[0]);
        }
        else if (strstr(body, "\"method\":\"textDocument/codeAction\"") != NULL)
        {
            if (opened != 3U || queried || strstr(body, uris[0]) == NULL ||
                (diagnostics && strstr(body, "same source session") == NULL) ||
                (!diagnostics && strstr(body, "\"diagnostics\":[]") == NULL))
            {
                result = 10;
                break;
            }
            queried = 1;
            if (strcmp(mode, "actions-sources-query-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"refused\"}}", id);
            else
            {
                const char *edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                   "\"character\":6}},\"newText\":\"renamed\"}]";
                if (strcmp(mode, "actions-sources-outside") == 0)
                    edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":99}},\"newText\":\"renamed\"}]";
                int size;
                if (strcmp(mode, "actions-sources-all") == 0)
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s,\"%s\":%s,\"%s\":%s}}",
                                 uris[0], edit, uris[1], edit, uris[2], edit);
                else if (strcmp(mode, "actions-sources-stale") == 0)
                    size = snprintf(proposal, sizeof(proposal),
                                    "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":2},"
                                    "\"edits\":%s}]}",
                                    uris[1], edit);
                else
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}",
                                 strcmp(mode, "actions-sources-unknown") == 0 ? "file:///workspace/unknown.c"
                                                                              : uris[1],
                                 edit);
                if (size < 0 || (size_t)size >= sizeof(proposal))
                {
                    result = 11;
                    break;
                }
                const char *metadata = strcmp(mode, "actions-sources-disabled") == 0
                                           ? ",\"disabled\":{\"reason\":\"unavailable\"}"
                                       : strcmp(mode, "actions-sources-command") == 0
                                           ? ",\"command\":{\"title\":\"Run\",\"command\":\"never run\"}"
                                           : "";
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Review "
                                   "complete fix\",\"edit\":%s%s}]}",
                                   id, proposal, metadata);
                if (strcmp(mode, "actions-sources-choices") == 0)
                    written =
                        snprintf(response, sizeof(response),
                                 "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":[{\"title\":\"Unavailable "
                                 "choice\",\"disabled\":{\"reason\":\"choose "
                                 "another\"},\"edit\":{}},{\"title\":\"Complete choice\",\"edit\":%s}]}",
                                 id, proposal);
            }
        }
        else if (strstr(body, "\"method\":\"textDocument/didClose\"") != NULL)
        {
            int match = -1;
            for (unsigned i = 0U; i < 3U; ++i)
                if (strstr(body, uris[i]) != NULL)
                    match = (int)i;
            if (!queried || match < 0 || !remaining[match])
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
            if (strcmp(mode, "actions-sources-shutdown-error") == 0)
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
