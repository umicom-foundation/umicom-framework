/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/rename_sources_fixture.inc
 * PURPOSE: Independently verify multiple draft opens precede rename and all are closed before shutdown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This peer uses literal fixture messages rather than the production decoder,
 * so a shared serialization mistake cannot make both sides agree by accident. */
static int RenameSourcesFixture(const char *mode)
{
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    char line[512], response[8192], proposal[6144];
    unsigned stage = 0U, opened = 0U, closed = 0U;
    unsigned char remaining[3] = {0};
    int prepared = 0, queried = 0, result = 0;
    int prepare = strcmp(mode, "rename-sources-prepare") == 0;
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
                         "\"textDocumentSync\":{\"openClose\":true,\"change\":1},\"renameProvider\":%s}}}",
                         id, prepare ? "{\"prepareProvider\":true}" : "true");
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
            if (opened == 1U && strcmp(mode, "rename-sources-unicode") == 0 &&
                strstr(body, "caf\xc3\xa9") == NULL)
            {
                result = 8;
                break;
            }
            remaining[opened] = 1U;
            ++opened;
        }
        else if (strstr(body, "\"method\":\"textDocument/prepareRename\"") != NULL)
        {
            if (!prepare || opened != 3U || prepared || queried || strstr(body, uris[0]) == NULL)
            {
                result = 9;
                break;
            }
            prepared = 1;
            written = snprintf(response, sizeof(response),
                               "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"start\":{\"line\":0,"
                               "\"character\":0},\"end\":{\"line\":0,\"character\":6}}}",
                               id);
        }
        else if (strstr(body, "\"method\":\"textDocument/rename\"") != NULL)
        {
            if (opened != 3U || queried || (prepare && !prepared) || strstr(body, uris[0]) == NULL ||
                strstr(body, "\"newName\":\"renamed\"") == NULL)
            {
                result = 10;
                break;
            }
            queried = 1;
            if (strcmp(mode, "rename-sources-query-error") == 0)
                written = snprintf(
                    response, sizeof(response),
                    "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-1,\"message\":\"refused\"}}", id);
            else
            {
                const char *edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                                   "\"character\":6}},\"newText\":\"renamed\"}]";
                if (strcmp(mode, "rename-sources-outside") == 0)
                    edit = "[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
                           "\"character\":99}},\"newText\":\"renamed\"}]";
                int size;
                if (strcmp(mode, "rename-sources-all") == 0)
                    size =
                        snprintf(proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s,\"%s\":%s,\"%s\":%s}}",
                                 uris[0], edit, uris[1], edit, uris[2], edit);
                else if (strcmp(mode, "rename-sources-stale") == 0)
                    size = snprintf(proposal, sizeof(proposal),
                                    "{\"documentChanges\":[{\"textDocument\":{\"uri\":\"%s\",\"version\":2},"
                                    "\"edits\":%s}]}",
                                    uris[1], edit);
                else
                    size = snprintf(
                        proposal, sizeof(proposal), "{\"changes\":{\"%s\":%s}}",
                        strcmp(mode, "rename-sources-unknown") == 0 ? "file:///workspace/unknown.c" : uris[1],
                        edit);
                if (size < 0 || (size_t)size >= sizeof(proposal))
                {
                    result = 11;
                    break;
                }
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":%s}", id, proposal);
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
            if (strcmp(mode, "rename-sources-shutdown-error") == 0)
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
