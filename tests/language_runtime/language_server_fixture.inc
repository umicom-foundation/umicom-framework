/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/language_server_fixture.inc
 * PURPOSE: Supply an independent small stdio language peer for native initialization and shutdown checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This fixture deliberately avoids the production frame parser. It accepts
 * only the small requests emitted by these tests, so a matching client bug
 * cannot be hidden by using the same parser on both ends. */
static int LanguageFixture(const char *mode)
{
    char line[512];
    char *body = malloc(65536U);
    if (body == NULL)
        return 3;
    int result = 0, initialized = 0;
    for (;;)
    {
        size_t length = 0U;
        int haveLength = 0;
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
                haveLength = 1;
            }
            if (strcmp(line, "\r\n") == 0)
                break;
            if (fgets(line, sizeof(line), stdin) == NULL)
            {
                result = 4;
                goto done;
            }
        }
        if (!haveLength || fread(body, 1U, length, stdin) != length)
        {
            result = 4;
            break;
        }
        body[length] = '\0';
        if (strcmp(mode, "lsp-timeout") == 0)
            continue;
        const char *idText = strstr(body, "\"id\":");
        unsigned long long id = idText == NULL ? 0ULL : strtoull(idText + 5U, NULL, 10);
        char response[512];
        int written = 0;
        if (strstr(body, "\"method\":\"initialize\"") != NULL)
        {
            if (strcmp(mode, "lsp-error") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"error\":{\"code\":-32603,\"message\":"
                                   "\"fixture refused\"}}",
                                   id);
            else if (strcmp(mode, "lsp-invalid") == 0)
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
            else
                written = snprintf(response, sizeof(response),
                                   "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                                   "\"completionProvider\":{},\"hoverProvider\":true}}}",
                                   id);
        }
        else if (strstr(body, "\"method\":\"initialized\"") != NULL)
            initialized = 1;
        else if (strstr(body, "\"method\":\"shutdown\"") != NULL)
        {
            if (!initialized)
            {
                result = 5;
                break;
            }
            written =
                snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}", id);
        }
        else if (strstr(body, "\"method\":\"exit\"") != NULL)
            break;
        else
        {
            result = 6;
            break;
        }
        if (written > 0)
        {
            if ((size_t)written >= sizeof(response) || printf("Content-Length: %d\r\n\r\n", written) < 0 ||
                fwrite(response, 1U, (size_t)written, stdout) != (size_t)written || fflush(stdout) != 0)
            {
                result = 7;
                break;
            }
        }
    }
done:
    free(body);
    return result;
}
